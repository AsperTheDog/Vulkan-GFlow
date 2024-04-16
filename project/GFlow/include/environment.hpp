#pragma once
#include <string_view>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "utils/identifiable.hpp"
#include "project.hpp"

namespace gflow
{
	class Environment : public Identifiable
	{
	public:

		void loadProject(std::string_view path);
		void addSurface(VkSurfaceKHR surface);

		void build(uint32_t gpuOverride = UINT32_MAX);

		[[nodiscard]] const Project& getProject(uint32_t id) const;
		[[nodiscard]] uint32_t getDevice() const;
		
		void configurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize);
		void configurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize, VkSurfaceFormatKHR format);
		void present(VkSurfaceKHR surface, uint32_t project, uint32_t image);

	private:
		uint32_t m_device = UINT32_MAX;

		uint32_t m_commandBuffer = UINT32_MAX;

		std::unordered_map<VkSurfaceKHR, uint32_t> m_swapchains{};

		std::vector<Project> m_projects{};
	};
} // namespace gflow