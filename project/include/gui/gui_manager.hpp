#pragma once
#include "backends/imgui_impl_vulkan.h"

class SDLWindow;

class GUIManager
{
public:
    static void init(const SDLWindow& window, ImGui_ImplVulkan_InitInfo& info);
    static void shutdown();

    static void render();

private:
    static void setImGuiColors();
};
