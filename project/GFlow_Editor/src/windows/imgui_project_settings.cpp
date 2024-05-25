#include "imgui_project_settings.hpp"

#include "imgui.h"

ImGuiProjectSettingsWindow::ImGuiProjectSettingsWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

char text[1000] = "";
void ImGuiProjectSettingsWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    ImGui::InputTextMultiline("##temp", text, 1000, {500, 500});
    ImGui::End();
}
