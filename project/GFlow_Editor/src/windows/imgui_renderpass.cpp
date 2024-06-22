#include "imgui_renderpass.hpp"

#include "imgui.h"
#include "resource_manager.hpp"
#include "nodes/renderpass/init_renderpass.hpp"
#include "nodes/renderpass/subpass_node.hpp"
#include "resources/render_pass.hpp"

ImGuiRenderPassWindow::ImGuiRenderPassWindow(const std::string_view& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
{
    m_grid.rightClickPopUpContent([this](ImFlow::BaseNode* node){this->rightClick(node);});

    m_grid.addNode<InitRenderpassNode>(ImVec2(1, 1));
}

void ImGuiRenderPassWindow::resourceSelected(const std::string& resource)
{
    if (!gflow::parser::ResourceManager::hasResource(resource) || gflow::parser::ResourceManager::getResourceType(resource) != gflow::parser::RenderPass::getTypeStatic())
        m_selectedPass = nullptr;
    else
        m_selectedPass = &gflow::parser::ResourceManager::getResource(resource);
}

void ImGuiRenderPassWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    if (m_selectedPass != nullptr)
        m_grid.update();
    ImGui::End();
    m_sidePanel.draw();
}

void ImGuiRenderPassWindow::rightClick(ImFlow::BaseNode* node)
{
    if (node == nullptr)
    {
        if (ImGui::BeginMenu("Add node"))
        {
            if (ImGui::BeginMenu("Structure"))
            {
                if (ImGui::MenuItem("Subpass"))
                {
                    m_grid.placeNode<SubpassNode>();
                }
                if (ImGui::MenuItem("Pipeline"))
                {

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Attachment"))
                {

                }
            }
            ImGui::EndMenu();
        }
    }
    else if (GFlowNode* gfNode = dynamic_cast<GFlowNode*>(node); gfNode != nullptr)
    {
        if (ImGui::MenuItem("Inspect"))
        {
            if (m_sidePanelTarget != nullptr)
                m_sidePanelTarget->setInspectionStatus(false);
            m_sidePanelTarget = gfNode;
            m_sidePanel.resourceSelected(m_sidePanelTarget->getLinkedResource());
            m_sidePanelTarget->setInspectionStatus(true);
        }
    }
}
