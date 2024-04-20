#pragma once
#include <string_view>
#include <vector>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "utils/signal.hpp"

class VulkanFence;
class VulkanDevice;
class VulkanContext;
class VulkanQueue;

class SDLWindow
{
public:
	struct WindowSize
	{
		uint32_t width;
		uint32_t height;

		[[nodiscard]] VkExtent2D toExtent2D() const;
		WindowSize(uint32_t width, uint32_t height);
		WindowSize(Sint32 width, Sint32 height);
	};

	SDLWindow() = default;
	SDLWindow(std::string_view name, int width, int height, int top = SDL_WINDOWPOS_CENTERED, int left = SDL_WINDOWPOS_CENTERED, uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

	[[nodiscard]] bool shouldClose() const;
	[[nodiscard]] std::vector<const char*> getRequiredVulkanExtensions() const;
	[[nodiscard]] WindowSize getSize() const;

	void pollEvents();

	void createSurface(VkInstance instance);

	void initImgui() const;
	void frameImgui() const;

	SDL_Window* operator*() const;
	[[nodiscard]] VkSurfaceKHR getSurface() const;

	void free();
	void shutdownImgui() const;

	[[nodiscard]] Signal<uint32_t, uint32_t>& getResizeSignal();

private:
	SDL_Window* m_SDLHandle = nullptr;

	VkInstance m_instance = nullptr;
	VkSurfaceKHR m_surface = nullptr;

	Signal<uint32_t, uint32_t> m_resizeSignal;

	friend class Surface;
	friend class VulkanGPU;
};

