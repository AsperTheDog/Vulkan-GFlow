#include "environment.hpp"

#include <stdexcept>

#include "vulkan_context.hpp"
#include "utils/logger.hpp"


namespace gflow
{
	void Environment::loadProject(std::string_view path)
	{
	}

	void Environment::addSurface(const VkSurfaceKHR surface)
	{
		m_swapchains.emplace(surface, UINT32_MAX);
	}

	void Environment::build(uint32_t gpuOverride)
	{
		
	}

	const Project& Environment::getProject(const uint32_t id) const
	{
		for (const Project& project : m_projects)
		{
			if (project.getID() == id)
			{
				return project;
			}
		}

		Logger::print("Project search failed out of " + std::to_string(m_projects.size()) + " projects", Logger::LevelBits::DEBUG);
		throw std::runtime_error("Project (ID: " + std::to_string(id) + ") not found");
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
} // namespace gflow