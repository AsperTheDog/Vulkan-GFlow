#include "imgui_execution.hpp"

#include "imgui.h"

ImGuiExecutionWindow::ImGuiExecutionWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
    m_grid.rightClickPopUpContent([this](const ImFlow::BaseNode* node){this->rightClick(node);});
}

void ImGuiExecutionWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    m_grid.update();
    ImGui::End();
}

void ImGuiExecutionWindow::rightClick(const ImFlow::BaseNode* node) const
{
    if (node == nullptr)
    {
        ImGui::Text("Testing");
    }
    else
    {
        ImGui::Text("Testing on node");
    }
}
