#include "sdl_window.hpp"

#include <stdexcept>
#include <SDL2/SDL.h>

#include "backends/imgui_impl_sdl2.h"
#include "utils/logger.hpp"

VkExtent2D SDLWindow::WindowSize::toExtent2D() const
{
    return { width, height };
}

SDLWindow::WindowSize::WindowSize(const uint32_t width, const uint32_t height)
    : width(width), height(height)
{

}

SDLWindow::WindowSize::WindowSize(const Sint32 width, const Sint32 height)
    : width(static_cast<uint32_t>(width)), height(static_cast<uint32_t>(height))
{

}

SDLWindow::SDLWindow(const std::string&name, const int width, const int height, const int top, const int left, const uint32_t flags)
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
    return { width, height };
}

bool SDLWindow::isMinimized() const
{
    return m_minimized;
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
                m_resizeSignal.emit(event.window.data1, event.window.data2);
                m_minimized = false;
            }
            else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
            {
                m_minimized = true;
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_s && (event.key.keysym.mod & KMOD_CTRL))
            {
                m_saveInput.emit();
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

void SDLWindow::freeSurface()
{
    if (m_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        Logger::print(Logger::DEBUG, "Freed surface (ID: ", m_surface, ")");
        m_surface = VK_NULL_HANDLE;
    }
}

void SDLWindow::free()
{
    freeSurface();

    SDL_DestroyWindow(m_SDLHandle);
    Logger::print(Logger::DEBUG, "Freed SDLHandle window (ID: ", m_SDLHandle, ")");
    SDL_Quit();
    m_SDLHandle = nullptr;
}

void SDLWindow::shutdownImgui() const
{
    ImGui_ImplSDL2_Shutdown();
}

Signal<uint32_t, uint32_t>& SDLWindow::getResizeSignal()
{
    return m_resizeSignal;
}

Signal<>& SDLWindow::getSaveInputSignal()
{
    return m_saveInput;
}

void SDLWindow::initImgui() const
{
    ImGui_ImplSDL2_InitForVulkan(m_SDLHandle);
}

void SDLWindow::frameImgui() const
{
    ImGui_ImplSDL2_NewFrame();
}
