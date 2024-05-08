#pragma once
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "utils/identifiable.hpp"
#include "project.hpp"
#include "vulkan_gpu.hpp"
#include "vulkan_queues.hpp"

namespace gflow
{
	class Environment : public Identifiable
	{
	public:

		uint32_t loadProject(std::string_view path);
		void addSurface(VkSurfaceKHR surface);

		void build(uint32_t gpuOverride = UINT32_MAX);

		Project& getProject(uint32_t id);
		[[nodiscard]] const Project& getProject(uint32_t id) const;

		void beginRecording(const std::vector<VkSurfaceKHR>& surfacesToPrepare = {});
		void recordProject(uint32_t project);
		void setRecordingBarrier() const;
		void endRecording();

		void configurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize);
		void configurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize, VkSurfaceFormatKHR format);
		void reconfigurePresentTarget(VkSurfaceKHR surface, VkExtent2D windowSize);
		bool present(VkSurfaceKHR surface);
		
		[[nodiscard]] uint32_t man_getCommandBuffer() const;
		[[nodiscard]] uint32_t man_getDevice() const;
		[[nodiscard]] QueueSelection man_getQueuePos(QueueFamilyTypeBits type) const;
		void man_manualBuild(const Project::Requirements& requirements, uint32_t gpuOverride = UINT32_MAX);
		uint32_t man_acquireSwapchainImage(VkSurfaceKHR surface);

		uint32_t man_getSwapchainImage(VkSurfaceKHR surface);
		uint32_t man_getSwapchain(VkSurfaceKHR surface);

	private:
		Environment() = default;
		void destroy();

		[[nodiscard]] Project::Requirements getRequirements() const;
		//bool blitImage(VkSurfaceKHR surface, uint32_t deviceImage);

		uint32_t m_device = UINT32_MAX;

		struct Swapchain
		{
			uint32_t id = UINT32_MAX;
			QueueSelection presentQueue{};
			bool mustBeAwaited = false;
		};
		VkSurfaceKHR m_surfaceToPresent = VK_NULL_HANDLE;
		
		uint32_t m_inFlightFence = UINT32_MAX;
		uint32_t m_renderFinishedSemaphoreID = UINT32_MAX;
		uint32_t m_readyToPresentSemaphoerID = UINT32_MAX;

		std::unordered_map<VkSurfaceKHR, Swapchain> m_swapchains{};
		std::vector<Project> m_projects{};
		
		uint32_t m_commandBuffer = UINT32_MAX;
		uint32_t m_transferBuffer = UINT32_MAX;
		QueueSelection m_mainQueue{};
		QueueSelection m_transferQueue{};

		bool isGPUSuitable(VulkanGPU gpu, const Project::Requirements& requirements);
		VulkanGPU selectGPU(const Project::Requirements& requirements);

		friend class Context;
	};
} // namespace gflow