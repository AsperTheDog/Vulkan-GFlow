#include "imgui_project_settings.hpp"

#include "imgui.h"

ImGuiProjectSettingsWindow::ImGuiProjectSettingsWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiProjectSettingsWindow::draw()
{
    ImGui::Begin(name.c_str(), &open);

    ImGui::End();
}
