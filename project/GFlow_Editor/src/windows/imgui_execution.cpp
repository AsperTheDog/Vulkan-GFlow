#include "imgui_execution.hpp"

#include "imgui.h"

ImGuiExecutionWindow::ImGuiExecutionWindow(const std::string_view& name, const bool defaultOpen)
: ImGuiGraphWindow(name, defaultOpen)
{}

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
