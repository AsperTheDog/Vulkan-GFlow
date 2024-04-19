#include "environment.hpp"

#include <ranges>
#include <stdexcept>
#include <thread>

#include "vulkan_context.hpp"
#include "utils/logger.hpp"

namespace gflow
{
	uint32_t Environment::loadProject(std::string_view path)
	{
		m_projects.push_back({ path, m_id });
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
			Logger::print("Manually selected GPU with id " + std::to_string(gpuOverride), Logger::INFO);
			const std::vector<VulkanGPU> gpus = VulkanContext::getGPUs();
			selectedGPU = gpus[gpuOverride];
			if (!isGPUSuitable(selectedGPU, requirements))
			{
				Logger::print("Invalid GPU: manually selected GPU is not suitable, searching alternative...", Logger::WARN);
				selectedGPU = selectGPU(requirements);
			}
		}
		else
		{
			Logger::print("Automatically selecting GPU", Logger::INFO);
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

		std::vector<const char*> extensions{};
		extensions.reserve(requirements.extensions.size());
		for (const std::string& extension : requirements.extensions) extensions.push_back(extension.c_str());

		m_device = VulkanContext::createDevice(selectedGPU, selector, extensions, requirements.features);

		QueueFamily queueFamily = queueStructure.getQueueFamily(m_mainQueue.familyIndex);
		m_commandBuffer = VulkanContext::getDevice(m_device).createCommandBuffer(queueFamily, 0, false);

		QueueFamily transferFamily = queueStructure.getQueueFamily(m_transferQueue.familyIndex);
		m_transferBuffer = VulkanContext::getDevice(m_device).createCommandBuffer(transferFamily, 0, false);

		m_inFlightFence = VulkanContext::getDevice(m_device).createFence(VK_FENCE_CREATE_SIGNALED_BIT);
		m_renderFinishedSemaphoreID = VulkanContext::getDevice(m_device).createSemaphore();
		m_readyToPresentSemaphoerID = VulkanContext::getDevice(m_device).createSemaphore();

		Logger::popContext();
	}

	uint32_t Environment::man_acquireSwapchainImage(const VkSurfaceKHR surface)
	{
		if (!m_swapchains.contains(surface))
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(m_id));

		VulkanSwapchain& swapchain = VulkanContext::getDevice(m_device).getSwapchain(m_swapchains[surface].id);
		return swapchain.acquireNextImage();
	}

	uint32_t Environment::man_getSwapchainImage(const VkSurfaceKHR surface)
	{
		if (!m_swapchains.contains(surface))
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(m_id));

		return VulkanContext::getDevice(m_device).getSwapchain(m_swapchains[surface].id).getNextImage();
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

		Logger::print("Project search failed out of " + std::to_string(m_projects.size()) + " projects", Logger::LevelBits::DEBUG);
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

	void Environment::recordProject(uint32_t project)
	{
		throw std::runtime_error("Not implemented");
	}

	void Environment::setRecordingBarrier() const
	{
		VulkanContext::getDevice(m_device).getCommandBuffer(m_commandBuffer, 0).cmdSimpleAbsoluteBarrier();
	}

	void Environment::endRecording()
	{
		VulkanDevice& device = VulkanContext::getDevice(m_device);

		VulkanCommandBuffer& commandBuffer = device.getCommandBuffer(m_commandBuffer, 0);
		commandBuffer.endRecording();
		const VulkanQueue graphicsQueue = device.getQueue(m_mainQueue);

		std::vector<std::pair<uint32_t, VkSemaphoreWaitFlags>> semaphores{};
		for (const Swapchain& swapchain : m_swapchains | std::views::values)
		{
			if (swapchain.mustBeAwaited)
			{
				VulkanSwapchain& swapchainObj = device.getSwapchain(swapchain.id);
				semaphores.emplace_back(swapchainObj.getImgSemaphore(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
			}
		}

		const uint32_t fence = !semaphores.empty() ? m_inFlightFence : UINT32_MAX;
		commandBuffer.submit(graphicsQueue, semaphores, { m_renderFinishedSemaphoreID }, fence);
	}

	void Environment::configurePresentTarget(const VkSurfaceKHR surface, const VkExtent2D windowSize)
	{
		configurePresentTarget(surface, windowSize, VulkanContext::getDevice(m_device).getGPU().getFirstFormat(surface));
	}

	void Environment::configurePresentTarget(const VkSurfaceKHR surface, const VkExtent2D windowSize, const VkSurfaceFormatKHR format)
	{
		if (!m_swapchains.contains(surface))
		{
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(m_id));
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

		VulkanDevice& device = VulkanContext::getDevice(m_device);
		m_swapchains[surface].id = device.createSwapchain(surface, extent, format);
	}

	bool Environment::present(VkSurfaceKHR surface)
	{
		bool finishedCorrectly = true;
		for (auto& swapchain : m_swapchains | std::views::values)
		{
			
			if (swapchain.mustBeAwaited)
			{
				VulkanSwapchain& swapchainObj = VulkanContext::getDevice(m_device).getSwapchain(swapchain.id);
				if (!swapchainObj.present(swapchain.presentQueue, { m_renderFinishedSemaphoreID }))
				{
					finishedCorrectly = false;
					Logger::print("Swapchain (ID: " + std::to_string(swapchain.id) + ") out of date", Logger::WARN);
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

		for (const Swapchain& swapchain : m_swapchains | std::views::values)
			VulkanContext::getDevice(m_device).freeSwapchain(swapchain.id);
		m_swapchains.clear();

		VulkanContext::freeDevice(m_device);
		m_device = UINT32_MAX;
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
		Logger::print(std::string("Checking GPU: ") + gpu.getProperties().deviceName, Logger::DEBUG);

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
				Logger::print("Invalid GPU: unsupported requested features", Logger::INFO);
				return false;
			}
		}

		// Check queue families
		const VkQueueFlags queueFlags = requirements.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
		if (!queueStructure.areQueueFlagsSupported(queueFlags, true) || !queueStructure.areQueueFlagsSupported(requirements.queueFlags))
		{
			Logger::print("Invalid GPU: unsupported requested queue flags", Logger::INFO);
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
				Logger::print("Invalid GPU: unsupported present queue", Logger::INFO);
				return false;
			}
		}


		// Check extensions
		if (!requirements.extensions.empty())
		{
			const std::vector<VkExtensionProperties> extensions = gpu.getSupportedExtensions();
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
				Logger::print("Invalid GPU: unsupported requested extensions", Logger::INFO);
				return false;
			}
		}

		return true;
	}

	VulkanGPU Environment::selectGPU(const Project::Requirements& requirements)
	{
		Logger::pushContext("GPU Selection");
		std::vector<VulkanGPU> suitableGPUs{};
		for (VulkanGPU& gpu : VulkanContext::getGPUs())
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
						scores[i] += memory.size / (10ULL * 1024 * 1024);
					}
				}
			}

			Logger::print(std::string("GPU ") + +gpu.getProperties().deviceName + " score: " + std::to_string(scores[i]), Logger::DEBUG);
		}

		uint32_t bestGPU = 0;
		for (uint32_t i = 1; i < suitableGPUs.size(); ++i)
		{
			if (scores[i] > scores[bestGPU])
			{
				bestGPU = i;
			}
		}

		Logger::print(std::string("Selected GPU: ") + suitableGPUs[bestGPU].getProperties().deviceName, Logger::INFO);
		Logger::popContext();
		return suitableGPUs[bestGPU];
	}

	/*bool Environment::blitImage(const VkSurfaceKHR surface, const uint32_t deviceImage)
	{
		if (!m_swapchains.contains(surface))
			throw std::runtime_error("Surface not found in environment (ID: " + std::to_string(m_id));

		VulkanDevice& device = VulkanContext::getDevice(m_device);
		VulkanSwapchain& swapchain = device.getSwapchain(m_swapchains[surface].id);

		swapchain.acquireNextImage(swapchain.getImgSemaphore());

		if (swapchain.getNextImage() == UINT32_MAX)
		{
			Logger::print("Swapchain out of date", Logger::WARN);
			return false;
		}

		const VulkanImage& image = device.getImage(deviceImage);
		const VulkanImage& presentImage = swapchain.getImage(swapchain.getNextImage());

		const VulkanQueue& transferQueue = device.getQueue(m_transferQueue);
		const VulkanQueue& presentQueue = device.getQueue(m_swapchains[surface].presentQueue);

		VulkanCommandBuffer& commandBuffer = device.getCommandBuffer(m_transferBuffer, 0);
		commandBuffer.reset();
		commandBuffer.beginRecording();

		if (image.getLayout() != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
			commandBuffer.cmdSimpleTransitionImageLayout(deviceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, m_mainQueue.familyIndex, m_transferQueue.familyIndex);

		if (presentImage.getLayout() != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			commandBuffer.cmdSimpleTransitionImageLayout(swapchain.getNextImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

		commandBuffer.cmdSimpleBlitImage(image, presentImage, VK_FILTER_LINEAR);

		commandBuffer.cmdSimpleTransitionImageLayout(swapchain.getNextImage(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);

		commandBuffer.endRecording();
		commandBuffer.submit(transferQueue, {}, {m_readyToPresentSemaphoerID}, m_inFlightFence);

		return true;
	}*/
} // namespace gflow