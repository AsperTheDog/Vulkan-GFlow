#include "imgui_renderpass.hpp"

#include "editor.hpp"
#include "imgui.h"
#include "resource_manager.hpp"
#include "vulkan_shader.hpp"
#include "nodes/renderpass/img_node.hpp"
#include "nodes/renderpass/init_node.hpp"
#include "nodes/renderpass/subpass_node.hpp"
#include "nodes/renderpass/subpass_pipeline_node.hpp"
#include "resources/render_pass.hpp"

ImGuiRenderPassWindow::ImGuiRenderPassWindow(const std::string_view& name, const bool defaultOpen) : ImGuiGraphWindow(name, defaultOpen)
{
    m_grid.addNode<InitRenderpassNode>(ImVec2(1, 1), this);
}

void ImGuiRenderPassWindow::resourceSelected(const std::string& resource)
{
    if (gflow::parser::ResourceManager::hasResource(resource) && gflow::parser::ResourceManager::getResourceType(resource) == gflow::parser::RenderPass::getTypeStatic())
    {
        saveRenderPass();
        m_selectedPass = dynamic_cast<gflow::parser::RenderPass*>(gflow::parser::ResourceManager::getResource(resource));
        const std::string metaPath = m_selectedPass->getMetaPath();
        if (gflow::parser::ResourceManager::hasResource(metaPath))
            m_selectedPassMeta = dynamic_cast<RenderpassResource*>(gflow::parser::ResourceManager::getResource(metaPath));
        else
            m_selectedPassMeta = gflow::parser::ResourceManager::createResource<RenderpassResource>(metaPath);
        loadRenderPass();
    }
}

void ImGuiRenderPassWindow::resourceVariableChanged(const std::string& resourcePath, const std::string& field, const std::string& subpath)
{
    if (m_selectedPass == nullptr) return;
    gflow::parser::Resource* res = gflow::parser::ResourceManager::getSubresource(resourcePath, subpath);
    if (res == nullptr || res->isSubresource() || res->getType() != gflow::parser::Pipeline::getTypeStatic()) 
        return;
    VulkanShader::ReflectionData data;
    Editor::ShaderStage type;
    if (field == "vertex")
    {
        data = Editor::getShaderResourceReflectionData(res->getValue<std::string>(field), Editor::VERTEX);
        type = Editor::VERTEX;
    }
    else if (field == "fragment")
    {
        data = Editor::getShaderResourceReflectionData(res->getValue<std::string>(field), Editor::FRAGMENT);
        type = Editor::FRAGMENT;
    }
    else if (field == "geometry")
    {
        data = Editor::getShaderResourceReflectionData(res->getValue<std::string>(field), Editor::GEOMETRY);
        type = Editor::GEOMETRY;
    }
    else return;

    if (!data.valid) return;
    // TODO: Update renderpass with new shader elements
}

void ImGuiRenderPassWindow::draw()
{
    if (m_selectedPass != nullptr)
    {
        ImGuiGraphWindow::draw();
    }
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
                    SubpassNodeResource* resource = dynamic_cast<SubpassNodeResource*>(m_selectedPassMeta->addNode(RenderpassResource::SUBPASS));
                    const std::shared_ptr<SubpassNode> newNode = m_grid.placeNode<SubpassNode>(this, resource);
                    resource->setPos({newNode->getPos().x, newNode->getPos().y});
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                if (ImGui::MenuItem("Pipeline"))
                {
                    PipelineNodeResource* resource = dynamic_cast<PipelineNodeResource*>(m_selectedPassMeta->addNode(RenderpassResource::PIPELINE));
                    const std::shared_ptr<SubpassPipelineNode> newNode = m_grid.placeNode<SubpassPipelineNode>(this, resource);
                    resource->setPos({newNode->getPos().x, newNode->getPos().y});
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Attachment"))
                {
                    ImageNodeResource* resource = dynamic_cast<ImageNodeResource*>(m_selectedPassMeta->addNode(RenderpassResource::IMAGE));
                    const std::shared_ptr<ImageNode> newNode = m_grid.placeNode<ImageNode>(this, resource);
                    resource->setPos({newNode->getPos().x, newNode->getPos().y});
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }
    else ImGuiGraphWindow::rightClick(node);
}

void ImGuiRenderPassWindow::saveRenderPass()
{
    if (m_selectedPass == nullptr) return;
}

void ImGuiRenderPassWindow::loadRenderPass()
{

}
