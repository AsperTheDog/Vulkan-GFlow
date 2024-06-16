#include "imgui_renderpass.hpp"

#include "imgui.h"
#include "resource_manager.hpp"
#include "nodes/graphics_pass_node.hpp"
#include "resources/render_pass.hpp"

ImGuiRenderPassWindow::ImGuiRenderPassWindow(const std::string_view& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
{
    m_grid.rightClickPopUpContent([this](ImFlow::BaseNode* node){this->rightClick(node);});

    m_grid.addNode<InitRenderpassNode>({2, 2});
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
}

void ImGuiRenderPassWindow::rightClick(ImFlow::BaseNode* node)
{
    if (node == nullptr)
    {
        if (ImGui::BeginMenu("Add node"))
        {
            if (ImGui::MenuItem("Subpass"))
            {
                m_grid.placeNode<GraphicsPassNode, uint32_t>(0);
            }
            if (ImGui::MenuItem("Attachment reference"))
            {
                
            }
            ImGui::EndMenu();
        }
    }
    else
    {
        if (const InitRenderpassNode* init = dynamic_cast<InitRenderpassNode*>(node); init != nullptr)
        {
            ImGui::Text("Testing on init subpass node");
        }
        else
        {
            ImGui::Text("Testing on node");
        }
        
    }
}
