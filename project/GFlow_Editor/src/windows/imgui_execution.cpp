#include "imgui_execution.hpp"

#include "imgui.h"

ImGuiExecutionWindow::ImGuiExecutionWindow(const std::string_view& name, const bool defaultOpen)
: ImGuiGraphWindow(name, defaultOpen)
{
    m_refreshRequestedSignal.connect(this, &ImGuiExecutionWindow::buildProject);
}

void ImGuiExecutionWindow::buildProject()
{
}

void ImGuiExecutionWindow::rightClick(ImFlow::BaseNode* node)
{
    if (node == nullptr)
    {
        if (ImGui::BeginMenu("Add node"))
        {
            if (ImGui::BeginMenu("Renderpass"))
            {
                if (ImGui::MenuItem("Begin"))
                {
                    
                }
                if (ImGui::MenuItem("Next"))
                {

                }
                if (ImGui::MenuItem("End"))
                {

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Image"))
                {

                }
                if (ImGui::MenuItem("Model"))
                {

                }
                if (ImGui::BeginMenu("Camera"))
                {
                    if (ImGui::MenuItem("Flight"))
                    {

                    }
                    if (ImGui::MenuItem("Object"))
                    {

                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }
    else ImGuiGraphWindow::rightClick(node);
}
