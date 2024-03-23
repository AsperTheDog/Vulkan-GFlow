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

	void recreateSwapchainResources(VkExtent2D extent);
	void initImgui() const;

	void setupEvents();

	void drawImgui() const;

	Camera cam;
	SDLWindow m_window;

	uint32_t m_deviceID = UINT32_MAX;

	QueueSelection m_transferQueuePos{};

	std::vector<uint32_t> m_framebuffers{};
	uint32_t m_imageAvailableSemaphoreID = UINT32_MAX;
	uint32_t m_renderFinishedSemaphoreID = UINT32_MAX;
	uint32_t m_inFlightFenceID = UINT32_MAX;
};

