#include "imgui_execution.hpp"

#include "imgui.h"

ImGuiExecutionWindow::ImGuiExecutionWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiExecutionWindow::draw()
{
    ImGui::Begin(name.c_str(), &open);
    m_grid.update();
    ImGui::End();
}
