#include "imgui_renderpass.hpp"

#include "imgui.h"

ImGuiRenderPassWindow::ImGuiRenderPassWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiRenderPassWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    m_grid.update();
    ImGui::End();
}
