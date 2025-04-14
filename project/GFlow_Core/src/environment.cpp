#include "environment.hpp"

#include <ranges>
#include <stdexcept>
#include <thread>

#include "vulkan_context.hpp"
#include "vulkan_device.hpp"
#include "ext/vulkan_swapchain.hpp"
#include "utils/logger.hpp"

namespace gflow
{
	uint32_t Environment::loadProject(const std::string& path)
	{
		m_projects.push_back({ path, getID() });
		return m_projects.back().getID();
	}

	void Environment::addSurface(const VkSurfaceKHR surface)
	{
		m_swapchains.emplace(surface, Swapchain());
	}

	void Environment::man_manualBuild(const Project::Requirements& requirements, const uint32_t gpuOverride)
	{
		Logger::pushContext("Environment creation");
		VulkanGPU selectedGPU;
		if (gpuOverride < VulkanContext::getGPUCount())
		{
			Logger::print(Logger::INFO, "Manually selected GPU with id ", gpuOverride);
			std::array<VulkanGPU, 10> gpus;
		    VulkanContext::getGPUs(gpus.data());
			selectedGPU = gpus[gpuOverride];
			if (!isGPUSuitable(selectedGPU, requirements))
			{
				Logger::print(Logger::WARN, "Invalid GPU: manually selected GPU is not suitable, searching alternative...");
				selectedGPU = selectGPU(requirements);
			}
		}
		else
		{
			Logger::print(Logger::INFO, "Automatically selecting GPU");
			selectedGPU = selectGPU(requirements);
		}

		const GPUQueueStructure queueStructure = selectedGPU.getQueueFamilies();
		QueueFamilySelector selector{ queueStructure };
		{
			VkQueueFlags queueFlags = requirements.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
			const QueueFamily queueFamily = queueStructure.findQueueFamily(queueFlags);
			m_mainQueue = selector.getOrAddQueue(queueFamily, 1.0);

			if (requirements.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				selector.selectQueueFamily(queueFamily, QueueFamilyTypeBits::GRAPHICS);
			if (requirements.queueFlags & VK_QUEUE_COMPUTE_BIT)
				selector.selectQueueFamily(queueFamily, QueueFamilyTypeBits::COMPUTE);

			if (requirements.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				const QueueFamily transferQueueFamily = queueStructure.findQueueFamily(VK_QUEUE_TRANSFER_BIT);
				selector.selectQueueFamily(transferQueueFamily, QueueFamilyTypeBits::TRANSFER);
				m_transferQueue = selector.addQueue(transferQueueFamily, 1.0);
			}
			if (requirements.present)
			{
				for (auto& [surface, swapchain] : m_swapchains)
				{
					const QueueFamily presentQueueFamily = queueStructure.findPresentQueueFamily(surface);
					selector.selectQueueFamily(presentQueueFamily, QueueFamilyTypeBits::PRESENT);
					swapchain.presentQueue = selector.getOrAddQueue(presentQueueFamily, 1.0);
				}
			}
		}

        VulkanDeviceExtensionManager extensions{};
        extensions.addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME, new VulkanSwapchainExtension(m_device));

		m_device = VulkanContext::createDevice(selectedGPU, selector, &extensions, requirements.features);
        VulkanDevice& device = VulkanContext::getDevice(m_device);

		QueueFamily queueFamily = queueStructure.getQueueFamily(m_mainQueue.familyIndex);
        device.initializeCommandPool(queueFamily, 0, true);
		m_commandBuffer = device.createCommandBuffer(queueFamily, 0, false);

		QueueFamily transferFamily = queueStructure.getQueueFamily(m_transferQueue.familyIndex);
		m_transferBuffer = device.createCommandBuffer(transferFamily, 0, false);

		m_inFlightFence = device.createFence(VK_FENCE_CREATE_SIGNALED_BIT);
		m_renderFinishedSemaphoreID = device.createSemaphore();
		m_readyToPresentSemaphoerID = device.createSemaphore();

		Logger::popContext();
	}

	uint32_t Environment::man_acquireSwapchainImage(const VkSurfaceKHR surface)
	{
		if (!m_swapchains.contains(surface))
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(getID()));

        VulkanSwapchainExtension* swapchainExtension = VulkanSwapchainExtension::get(VulkanContext::getDevice(m_device));
		VulkanSwapchain& swapchain = swapchainExtension->getSwapchain(m_swapchains[surface].id);
		return swapchain.acquireNextImage();
	}

    VulkanShader::ReflectionManager Environment::man_getReflectionData(const std::string& shaderPath, const VkShaderStageFlagBits stage) const
    {
        try
        {
            const uint32_t shaderID = VulkanContext::getDevice(m_device).createShader(shaderPath, stage, true, {});
            const VulkanShader& shader = VulkanContext::getDevice(m_device).getShader(shaderID);
            VulkanShader::ReflectionManager reflectionData = shader.getReflectionData();
            VulkanContext::getDevice(m_device).freeShader(shaderID);
            return reflectionData;
        }
        catch (const std::runtime_error& e)
        {
            Logger::print(Logger::ERR, "Failed to get reflection data for shader ", shaderPath, ": ", e.what());
            return {};
        }
        
    }

    uint32_t Environment::man_getSwapchainImage(const VkSurfaceKHR surface)
	{
		if (!m_swapchains.contains(surface))
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(getID()));

        VulkanSwapchainExtension* swapchainExtension = VulkanSwapchainExtension::get(VulkanContext::getDevice(m_device));
		return swapchainExtension->getSwapchain(m_swapchains[surface].id).getNextImage();
	}

	void Environment::build(const uint32_t gpuOverride)
	{
		const Project::Requirements requirements = getRequirements();
		man_manualBuild(requirements, gpuOverride);
	}

	Project& Environment::getProject(const uint32_t id)
	{
		for (Project& project : m_projects)
		{
			if (project.getID() == id)
			{
				return project;
			}
		}

		Logger::print(Logger::DEBUG, "Project search failed out of ", m_projects.size(), " projects");
		throw std::runtime_error("Project (ID: " + std::to_string(id) + ") not found");
	}

	const Project& Environment::getProject(const uint32_t id) const
	{
		return const_cast<Environment*>(this)->getProject(id);
	}

	uint32_t Environment::man_getDevice() const
	{
		return m_device;
	}

	uint32_t Environment::man_getCommandBuffer() const
	{
		return m_commandBuffer;
	}

	void Environment::beginRecording(const std::vector<VkSurfaceKHR>& surfacesToPrepare)
	{
		VulkanDevice& device = VulkanContext::getDevice(m_device);

		device.getFence(m_inFlightFence).wait();
		device.getFence(m_inFlightFence).reset();

		for (const VkSurfaceKHR surface : surfacesToPrepare)
		{
			if (!m_swapchains.contains(surface)) continue;
			man_acquireSwapchainImage(surface);
			m_swapchains[surface].mustBeAwaited = true;
		}

		VulkanCommandBuffer& commandBuffer = device.getCommandBuffer(m_commandBuffer, 0);
		commandBuffer.reset();
		commandBuffer.beginRecording();
	}

	void Environment::recordProject(uint32_t project) const
    {
		throw std::runtime_error("Not implemented");
	}

	void Environment::setRecordingBarrier() const
	{
        VulkanMemoryBarrierBuilder barrierBuilder{m_device, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0};
        barrierBuilder.addAbsoluteMemoryBarrier();

		VulkanContext::getDevice(m_device).getCommandBuffer(m_commandBuffer, 0).cmdPipelineBarrier(barrierBuilder);
	}

	void Environment::endRecording()
	{
		VulkanDevice& device = VulkanContext::getDevice(m_device);

		VulkanCommandBuffer& commandBuffer = device.getCommandBuffer(m_commandBuffer, 0);
		commandBuffer.endRecording();
		const VulkanQueue graphicsQueue = device.getQueue(m_mainQueue);

		std::vector<VulkanCommandBuffer::WaitSemaphoreData> semaphores{};
		for (const Swapchain& swapchain : m_swapchains | std::views::values)
		{
			if (swapchain.mustBeAwaited)
			{
                VulkanSwapchainExtension* swapchainExtension = VulkanSwapchainExtension::get(device);
				VulkanSwapchain& swapchainObj = swapchainExtension->getSwapchain(swapchain.id);
				semaphores.emplace_back(swapchainObj.getImgSemaphore(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
			}
		}

		const uint32_t fence = !semaphores.empty() ? m_inFlightFence : UINT32_MAX;
		commandBuffer.submit(graphicsQueue, semaphores, {{ m_renderFinishedSemaphoreID }}, fence);
	}

	void Environment::configurePresentTarget(const VkSurfaceKHR surface, const VkExtent2D windowSize)
	{
		configurePresentTarget(surface, windowSize, VulkanContext::getDevice(m_device).getGPU().getFirstFormat(surface));
	}

	void Environment::configurePresentTarget(const VkSurfaceKHR surface, const VkExtent2D windowSize, const VkSurfaceFormatKHR format)
	{
		if (!m_swapchains.contains(surface))
		{
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(getID()));
		}

		const VkSurfaceCapabilitiesKHR capabilities = VulkanContext::getDevice(m_device).getGPU().getCapabilities(surface);
		VkExtent2D extent;
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			extent = capabilities.currentExtent;
		}
		else
		{
			extent.width = std::max(std::min(windowSize.width, capabilities.minImageExtent.width), capabilities.maxImageExtent.width);
			extent.height = std::max(std::min(windowSize.height, capabilities.minImageExtent.height), capabilities.maxImageExtent.height);
		}

        const VulkanDevice& device = VulkanContext::getDevice(m_device);
	    VulkanSwapchainExtension* swapchainExtension = VulkanSwapchainExtension::get(device);
		m_swapchains[surface].id = swapchainExtension->createSwapchain(surface, extent, format, VK_PRESENT_MODE_FIFO_KHR);
	}

	bool Environment::present(VkSurfaceKHR surface)
	{
		bool finishedCorrectly = true;
		for (auto& swapchain : m_swapchains | std::views::values)
		{
			
			if (swapchain.mustBeAwaited)
			{
                VulkanSwapchainExtension* swapchainExtension = VulkanSwapchainExtension::get(VulkanContext::getDevice(m_device));
				VulkanSwapchain& swapchainObj = swapchainExtension->getSwapchain(swapchain.id);
				if (!swapchainObj.present(swapchain.presentQueue, {{ m_renderFinishedSemaphoreID }}))
				{
					finishedCorrectly = false;
					Logger::print(Logger::WARN, "Swapchain (ID: ", swapchain.id, ") out of date");
				}
				swapchain.mustBeAwaited = false;
			}
		}
		return finishedCorrectly;
	}

	uint32_t Environment::man_getSwapchain(const VkSurfaceKHR surface)
	{
		return m_swapchains[surface].id;
	}

	QueueSelection Environment::man_getQueuePos(const QueueFamilyTypeBits type) const
	{
		switch (type)
		{
		case QueueFamilyTypeBits::GRAPHICS:
		case QueueFamilyTypeBits::COMPUTE:
			return m_mainQueue;
		case QueueFamilyTypeBits::TRANSFER:
			return m_transferQueue;
		default:
			return {};
		}

	}

	void Environment::destroy()
	{
		if (m_device == UINT32_MAX) return;

		VulkanContext::freeDevice(m_device);
		m_device = UINT32_MAX;
	}

	void Environment::reconfigurePresentTarget(const VkSurfaceKHR surface, const VkExtent2D windowSize)
	{
        const VulkanDevice& device = VulkanContext::getDevice(m_device);
        VulkanSwapchainExtension* swapchainExtension = VulkanSwapchainExtension::get(device);
		const VulkanSwapchain& swapchain = swapchainExtension->getSwapchain(m_swapchains[surface].id);
		m_swapchains[surface].id = swapchainExtension->createSwapchain(surface, windowSize, swapchain.getFormat(), VK_PRESENT_MODE_FIFO_KHR, m_swapchains[surface].id);
	}

    Project::Requirements Environment::getRequirements() const
	{
		Project::Requirements requirements;
		for (const Project& project : m_projects)
		{
			Project::Requirements projRequirements = project.getRequirements();
			requirements += project.getRequirements();
		}

		return requirements;
	}

	bool Environment::isGPUSuitable(const VulkanGPU gpu, const Project::Requirements& requirements)
	{
		Logger::print(Logger::DEBUG, "Checking GPU: ", gpu.getProperties().deviceName);

		bool invalid = false;
		const GPUQueueStructure queueStructure = gpu.getQueueFamilies();

		// Check features
		{
			const VkPhysicalDeviceFeatures features = gpu.getFeatures();

			const uint8_t* featurePtr = reinterpret_cast<const uint8_t*>(&features);
			const uint8_t* requestedFeaturePtr = reinterpret_cast<const uint8_t*>(&requirements.features);

			for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures); i++)
			{
				if ((requestedFeaturePtr[i] & featurePtr[i]) != requestedFeaturePtr[i])
				{
					invalid = true;
					break;
				}
			}
			if (invalid)
			{
				Logger::print(Logger::INFO, "Invalid GPU: unsupported requested features");
				return false;
			}
		}

		// Check queue families
		const VkQueueFlags queueFlags = requirements.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		if (!queueStructure.areQueueFlagsSupported(queueFlags, true) || !queueStructure.areQueueFlagsSupported(requirements.queueFlags))
		{
			Logger::print(Logger::INFO, "Invalid GPU: unsupported requested queue flags");
			return false;
		}


		// Check present
		if (requirements.present)
		{
			for (const auto& surface : m_swapchains | std::views::keys)
			{
				if (!queueStructure.isPresentSupported(surface))
				{
					invalid = true;
					break;
				}
			}
			if (invalid)
			{
				Logger::print(Logger::INFO, "Invalid GPU: unsupported present queue");
				return false;
			}
		}


		// Check extensions
		if (!requirements.extensions.empty())
		{
			std::vector<VkExtensionProperties> extensions;
            extensions.resize(gpu.getSupportedExtensionCount());
		    gpu.getSupportedExtensions(extensions.data());
			for (const std::string& extension : requirements.extensions)
			{
				bool found = false;
				for (const VkExtensionProperties& supportedExtension : extensions)
				{
					if (extension == supportedExtension.extensionName)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					invalid = true;
					break;
				}
			}
			if (invalid)
			{
				Logger::print(Logger::INFO, "Invalid GPU: unsupported requested extensions");
				return false;
			}
		}

		return true;
	}

	VulkanGPU Environment::selectGPU(const Project::Requirements& requirements)
	{
		Logger::pushContext("GPU Selection");
		std::vector<VulkanGPU> suitableGPUs{};
        std::vector<VulkanGPU> gpus{ VulkanContext::getGPUCount() };
        VulkanContext::getGPUs(gpus.data());
		for (VulkanGPU& gpu : gpus)
		{
			if (isGPUSuitable(gpu, requirements))
				suitableGPUs.push_back(gpu);
		}

		if (suitableGPUs.empty())
		{
			Logger::popContext();
			throw std::runtime_error("No suitable GPU found");
		}

		std::vector<uint32_t> scores(suitableGPUs.size(), 0);

		for (size_t i = 0; i < suitableGPUs.size(); ++i)
		{
			const VulkanGPU& gpu = suitableGPUs[i];

			// Discrete GPUs get automatic 10000 points
			if (gpu.getProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				scores[i] += 10000;
			}

			// Add 1 point for each 10MB of local memory
			{
				VkPhysicalDeviceMemoryProperties properties = gpu.getMemoryProperties();
				for (const VkMemoryHeap& memory : properties.memoryHeaps)
				{
					if (memory.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					{
						scores[i] += static_cast<uint32_t>(memory.size / (10ULL * 1024 * 1024));
					}
				}
			}

			Logger::print(Logger::DEBUG, "GPU ", gpu.getProperties().deviceName, " score: ", scores[i]);
		}

		uint32_t bestGPU = 0;
		for (uint32_t i = 1; i < suitableGPUs.size(); ++i)
		{
			if (scores[i] > scores[bestGPU])
			{
				bestGPU = i;
			}
		}

		Logger::print(Logger::INFO, "Selected GPU: ", suitableGPUs[bestGPU].getProperties().deviceName);
		Logger::popContext();
		return suitableGPUs[bestGPU];
	}
} // namespace gflow