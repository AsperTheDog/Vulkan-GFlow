#pragma once
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "utils/identifiable.hpp"
#include "project.hpp"
#include "vulkan_gpu.hpp"

namespace gflow
{
	class Environment : public Identifiable
	{
	public:

		uint32_t loadProject(std::string_view path);
		void addSurface(VkSurfaceKHR surface);

		void manualBuild(const Project::Requirements& requirements, uint32_t gpuOverride = UINT32_MAX);
		void build(uint32_t gpuOverride = UINT32_MAX);

		Project& getProject(uint32_t id);
		[[nodiscard]] const Project& getProject(uint32_t id) const;
		[[nodiscard]] uint32_t getDevice() const;
		
		void configurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize);
		void configurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize, VkSurfaceFormatKHR format);
		void present(VkSurfaceKHR surface, uint32_t project, uint32_t image);

	private:
		Environment() = default;

		[[nodiscard]] Project::Requirements getRequirements() const;

		uint32_t m_device = UINT32_MAX;

		uint32_t m_commandBuffer = UINT32_MAX;

		std::unordered_map<VkSurfaceKHR, uint32_t> m_swapchains{};
		std::vector<Project> m_projects{};

		VulkanGPU getSuitableGPU(const Project::Requirements& requirements);

		friend class Context;
	};
} // namespace gflow