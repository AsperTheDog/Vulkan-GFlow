#include "sdl_window.hpp"

#include <stdexcept>
#include <SDL2/SDL.h>

#include "backends/imgui_impl_sdl2.h"
#include "utils/logger.hpp"

VkExtent2D SDLWindow::WindowSize::toExtent2D() const
{
	return {width, height};
}

SDLWindow::WindowSize::WindowSize(const uint32_t width, const uint32_t height)
	: width(width), height(height)
{

}

SDLWindow::WindowSize::WindowSize(const Sint32 width, const Sint32 height)
	: width(static_cast<uint32_t>(width)), height(static_cast<uint32_t>(height))
{

}

SDLWindow::SDLWindow(const std::string_view name, const int width, const int height, const int top, const int left, const uint32_t flags)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	m_SDLHandle = SDL_CreateWindow(name.data(), top, left, width, height, flags | SDL_WINDOW_VULKAN);
}

bool SDLWindow::shouldClose() const
{
	return SDL_QuitRequested();
}

std::vector<const char*> SDLWindow::getRequiredVulkanExtensions() const
{
	uint32_t extensionCount;
	SDL_Vulkan_GetInstanceExtensions(m_SDLHandle, &extensionCount, nullptr);

	std::vector<const char*> extensions(extensionCount);
	SDL_Vulkan_GetInstanceExtensions(m_SDLHandle, &extensionCount, extensions.data());

	return extensions;
}

SDLWindow::WindowSize SDLWindow::getSize() const
{
	Sint32 width, height;
	SDL_GetWindowSize(m_SDLHandle, &width, &height);
	return {width, height};
}

void SDLWindow::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED && event.window.data1 > 0 && event.window.data2 > 0)
			{
				
			}
			break;
		}
	}
}

void SDLWindow::createSurface(const VkInstance instance)
{
	if (m_surface != nullptr) 
		throw std::runtime_error("Surface already created");

	if (SDL_Vulkan_CreateSurface(m_SDLHandle, instance, &m_surface) == SDL_FALSE)
		throw std::runtime_error("failed to create SDLHandle surface!");

	m_instance = instance;
}

SDL_Window* SDLWindow::operator*() const
{
	return m_SDLHandle;
}

VkSurfaceKHR SDLWindow::getSurface() const
{
	return m_surface;
}

void SDLWindow::free()
{
	if (m_surface != nullptr)
	{
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		m_surface = nullptr;
	}

	SDL_DestroyWindow(m_SDLHandle);
	SDL_Quit();
	m_SDLHandle = nullptr;
}

void SDLWindow::shutdownImgui()
{
	ImGui_ImplSDL2_Shutdown();
}

void SDLWindow::initImgui() const
{
	ImGui_ImplSDL2_InitForVulkan(m_SDLHandle);
}

void SDLWindow::frameImgui() const
{
	ImGui_ImplSDL2_NewFrame();
}
