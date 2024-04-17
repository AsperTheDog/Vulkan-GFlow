#include "environment.hpp"

#include <ranges>
#include <stdexcept>

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
		m_swapchains.emplace(surface, UINT32_MAX);
	}

	void Environment::manualBuild(const Project::Requirements& requirements, const uint32_t gpuOverride)
	{
		VulkanGPU selectedGPU;
		if (gpuOverride < VulkanContext::getGPUCount())
		{
			const std::vector<VulkanGPU> gpus = VulkanContext::getGPUs();
			selectedGPU = gpus[gpuOverride];
		}
		else
		{
			selectedGPU = getSuitableGPU(requirements);
		}

		const GPUQueueStructure queueStructure = selectedGPU.getQueueFamilies();
	}

	void Environment::build(const uint32_t gpuOverride)
	{
		const Project::Requirements requirements = getRequirements();
		manualBuild(requirements, gpuOverride);
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

	uint32_t Environment::getDevice() const
	{
		return m_device;
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
		m_swapchains[surface] = device.createSwapchain(surface, extent, format);
	}

	void Environment::present(VkSurfaceKHR surface, uint32_t project, uint32_t image)
	{
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

	VulkanGPU Environment::getSuitableGPU(const Project::Requirements& requirements)
	{
		std::vector<VulkanGPU> suitableGPUs{};
		for (VulkanGPU& gpu : VulkanContext::getGPUs())
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
					continue;
				}
			}

			// Check queue families
			if (!queueStructure.areQueueFlagsSupported(requirements.queueFlags))
			{
				Logger::print("Invalid GPU: unsupported requested queue flags", Logger::INFO);
				continue;
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
					continue;
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
					continue;
				}
			}

			suitableGPUs.push_back(gpu);
		}

		if (suitableGPUs.empty())
		{
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

			// If all flags are fulfilled a single queue family, add 1000 points
			{
				VkQueueFlags queueFlags = requirements.queueFlags & ~VK_QUEUE_TRANSFER_BIT;
				if (gpu.getQueueFamilies().areQueueFlagsSupported(queueFlags, true))
				{
					scores[i] += 1000;
				}
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

			Logger::print(std::string("GPU ") +  + gpu.getProperties().deviceName + " score: " + std::to_string(scores[i]), Logger::DEBUG);
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
		return suitableGPUs[bestGPU];
	}
} // namespace gflow