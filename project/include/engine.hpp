#pragma once
#include "camera.hpp"
#include "imgui.h"
#include "VkBase/sdl_window.hpp"
#include "VkBase/vulkan_queues.hpp"

class Engine
{
public:
	Engine();
	~Engine();

	void run();

private:
	void createDevice();
	void createSwapchain();
	void createSyncObjects();
	void createRenderPass();
	void createSwapchainFramebuffers(bool destroy);

	void recordCommandBuffer(uint32_t framebuffer, ImDrawData* im_draw_data) const;

	void recreateSwapchainResources(VkExtent2D newExtent);
	void initImgui() const;

	void setupEvents();

	Camera cam;
	SDLWindow m_window;

	uint32_t m_deviceID = UINT32_MAX;

	QueueSelection m_transferQueuePos{};
	QueueSelection m_graphicsImguiQueuePos{};
	QueueSelection m_graphicsQueuePos{};
	QueueSelection m_presentQueuePos{};
	
	uint32_t m_imguiRenderPassID = UINT32_MAX;

	std::vector<uint32_t> m_framebuffers{};
	uint32_t m_imageAvailableSemaphoreID = UINT32_MAX;
	uint32_t m_renderFinishedSemaphoreID = UINT32_MAX;
	uint32_t m_inFlightFenceID = UINT32_MAX;
	uint32_t m_graphicsCmdBufferID = UINT32_MAX;
};

