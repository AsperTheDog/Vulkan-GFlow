#include "imgui_execution.hpp"

#include "imgui.h"
#include "nodes/test_node.hpp"

ImGuiExecutionWindow::ImGuiExecutionWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
    m_grid.rightClickPopUpContent([this](ImFlow::BaseNode* node){this->rightClick(node);});
    m_grid.addNode<TestNode>({0, 0});
    m_grid.addNode<TestNode>({0, 0});
}

void ImGuiExecutionWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    m_grid.update();
    ImGui::End();
}

void ImGuiExecutionWindow::rightClick(ImFlow::BaseNode* node)
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
