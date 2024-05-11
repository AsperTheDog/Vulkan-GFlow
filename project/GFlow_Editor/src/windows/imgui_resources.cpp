#include "imgui_resources.hpp"

ImGuiResourcesWindow::ImGuiResourcesWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiResourcesWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    
    ImGui::End();
}
