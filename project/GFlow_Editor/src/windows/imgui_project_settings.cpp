#include "imgui_project_settings.hpp"

#include "imgui.h"

ImGuiProjectSettingsWindow::ImGuiProjectSettingsWindow(const std::string& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
{
}

char text[1000] = "";
void ImGuiProjectSettingsWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    ImGui::InputTextMultiline("##temp", text, 1000, {500, 500});
    ImGui::End();
}
