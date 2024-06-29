#include "imgui_renderpass.hpp"

#include "imgui.h"
#include "resource_manager.hpp"
#include "nodes/renderpass/img_node.hpp"
#include "nodes/renderpass/init_renderpass.hpp"
#include "nodes/renderpass/subpass_node.hpp"
#include "nodes/renderpass/subpass_pipeline_node.hpp"
#include "resources/render_pass.hpp"

ImGuiRenderPassWindow::ImGuiRenderPassWindow(const std::string_view& name, const bool defaultOpen) : ImGuiGraphWindow(name, defaultOpen)
{
    m_grid.addNode<InitRenderpassNode>(ImVec2(1, 1), this);
}

void ImGuiRenderPassWindow::resourceSelected(const std::string& resource)
{
    if (!gflow::parser::ResourceManager::hasResource(resource) || gflow::parser::ResourceManager::getResourceType(resource) != gflow::parser::RenderPass::getTypeStatic())
        m_selectedPass = nullptr;
    else
        m_selectedPass = &gflow::parser::ResourceManager::getResource(resource);
}

void ImGuiRenderPassWindow::onNodeDestroyed(GFlowNode* node)
{
    if (node == m_sidePanelTarget)
    {
        m_sidePanelTarget->setInspectionStatus(false);
        m_sidePanelTarget = nullptr;
    }
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
                    const std::shared_ptr<SubpassNode> newNode = m_grid.placeNode<SubpassNode>(this);
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                if (ImGui::MenuItem("Pipeline"))
                {
                    const std::shared_ptr<SubpassPipelineNode> newNode = m_grid.placeNode<SubpassPipelineNode>(this);
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Attachment"))
                {
                    const std::shared_ptr<RenderpassImageNode> newNode = m_grid.placeNode<RenderpassImageNode>(this);
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }
    else ImGuiGraphWindow::rightClick(node);
    
}
