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
    m_sidePanel.getVariableChangedSignal().connect(this, &ImGuiRenderPassWindow::sidePanelVariableChanged);
}

void ImGuiRenderPassWindow::resourceSelected(const std::string& resource)
{
    if (gflow::parser::ResourceManager::hasResource(resource) && gflow::parser::ResourceManager::getResourceType(resource) == gflow::parser::RenderPass::getTypeStatic())
    {
        saveRenderPass();
        if (m_sidePanelTarget != nullptr)
        {
            m_sidePanelTarget->setInspectionStatus(false);
            m_sidePanelTarget = nullptr;
        }
        clearGrid();
        m_selectedPass = dynamic_cast<gflow::parser::RenderPass*>(gflow::parser::ResourceManager::getResource(resource));
        const std::string metaPath = m_selectedPass->getMetaPath();
        if (gflow::parser::ResourceManager::hasResource(metaPath))
        {
            m_selectedPassMeta = dynamic_cast<RenderpassResource*>(gflow::parser::ResourceManager::getResource(metaPath));
            loadRenderPass(true);
        }
        else
        {
            m_selectedPassMeta = gflow::parser::ResourceManager::createResource<RenderpassResource>(metaPath, nullptr, true);
            InitNodeResource* initNodeRes = m_selectedPassMeta->addNode<InitNodeResource>();
            const std::shared_ptr<InitRenderpassNode> newNode = m_grid.addNode<InitRenderpassNode>(ImVec2(5, 5), this, initNodeRes);
            initNodeRes->setPos({newNode->getPos().x, newNode->getPos().y});
            initNodeRes->setNodeID(newNode->getUID());
            loadRenderPass(false);
        }
    }
}

void ImGuiRenderPassWindow::resourceVariableChanged(const gflow::parser::ResourceElemPath& element)
{
    if (m_selectedPass == nullptr) return;
    gflow::parser::Resource* res = gflow::parser::ResourceManager::getSubresource(element.path, element.stackedPath);
    if (res == nullptr || res->isSubresource() || res->getType() != gflow::parser::Pipeline::getTypeStatic()) 
        return;

    Editor::ShaderStage type;
    if (element.element == "vertex")        type = Editor::VERTEX;
    else if (element.element == "fragment") type = Editor::FRAGMENT;
    else if (element.element == "geometry") type = Editor::GEOMETRY;
    else return;

    const VulkanShader::ReflectionData data = Editor::getShaderReflectionData(res->getValue<std::string>(element.element), type);

    if (!data.valid) return;
    // TODO: Update renderpass with new shader elements
}

void ImGuiRenderPassWindow::sidePanelVariableChanged(const gflow::parser::ResourceElemPath& element)
{
    if (element.element == "pipeline")
    {
        //TODO: Update renderpass with new pipeline
    }
}

void ImGuiRenderPassWindow::draw()
{
    if (m_selectedPass != nullptr)
    {
        ImGuiGraphWindow::draw();
    }
}

void ImGuiRenderPassWindow::save()
{
    saveRenderPass();
}

void ImGuiRenderPassWindow::onNodeDestroyed(GFlowNode* node)
{
    m_selectedPassMeta->removeNode(node);
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
                    SubpassNodeResource* resource = m_selectedPassMeta->addNode<SubpassNodeResource>();
                    const std::shared_ptr<SubpassNode> newNode = m_grid.placeNode<SubpassNode>(this, resource);
                    resource->setPos({newNode->getPos().x, newNode->getPos().y});
                    resource->setNodeID(newNode->getUID());
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                if (ImGui::MenuItem("Pipeline"))
                {
                    PipelineNodeResource* resource = m_selectedPassMeta->addNode<PipelineNodeResource>();
                    const std::shared_ptr<SubpassPipelineNode> newNode = m_grid.placeNode<SubpassPipelineNode>(this, resource);
                    resource->setPos({newNode->getPos().x, newNode->getPos().y});
                    resource->setNodeID(newNode->getUID());
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Attachment"))
                {
                    ImageNodeResource* resource = m_selectedPassMeta->addNode<ImageNodeResource>();
                    const std::shared_ptr<ImageNode> newNode = m_grid.placeNode<ImageNode>(this, resource);
                    resource->setPos({newNode->getPos().x, newNode->getPos().y});
                    resource->setNodeID(newNode->getUID());
                    newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }
    else ImGuiGraphWindow::rightClick(node);
}

void ImGuiRenderPassWindow::clearGrid()
{
    ImGuiGraphWindow::clearGrid();
}

void ImGuiRenderPassWindow::onNodeCreated(ImFlow::BaseNode* node)
{
    std::cout << "Node created" << '\n';
}

void ImGuiRenderPassWindow::onNodeDeleted(ImFlow::BaseNode* node)
{
    std::cout << "Node deleted" << '\n';
}

void ImGuiRenderPassWindow::onConnection(ImFlow::Pin* pin1, ImFlow::Pin* pin2)
{
    std::cout << "Connection created" << '\n';
}

void ImGuiRenderPassWindow::saveRenderPass()
{
    if (m_selectedPass == nullptr) return;
    for (std::shared_ptr<ImFlow::BaseNode>& val : m_grid.getNodes() | std::views::values)
    {
        GFlowNode* gnode = dynamic_cast<GFlowNode*>(val.get());
        if (gnode == nullptr) continue;
        NodeResource* resource = gnode->getLinkedResource();
        if (resource == nullptr) continue;
        resource->setPos({ gnode->getPos().x, gnode->getPos().y });
    }

    // Get connection
    for (const std::weak_ptr<ImFlow::Link>& connection : m_grid.getLinks())
    {
        const ImFlow::Link* link = connection.lock().get();
        const size_t leftUID = link->left()->getParent()->getUID();
        const size_t leftPin = link->left()->getFilterID();
        const size_t rightUID = link->right()->getParent()->getUID();
        const size_t rightPin = link->right()->getFilterID();
        m_selectedPassMeta->addConnection(leftUID, leftPin, rightUID, rightPin);
    }
}

void ImGuiRenderPassWindow::loadRenderPass(const bool loadInit)
{
    for (int i = 0; i < m_selectedPassMeta->getNodes().size(); ++i)
    {
        NodeResource* resource = m_selectedPassMeta->getNodes()[i];
        GFlowNode* newNode = nullptr;
        if (resource->getType() == "InitNodeResource" && loadInit)
            newNode = m_grid.placeNode<InitRenderpassNode>(this, resource).get();
        else if (resource->getType() == "SubpassNodeResource")
            newNode = m_grid.placeNode<SubpassNode>(this, resource).get();
        else if (resource->getType() == "PipelineNodeResource")
            newNode = m_grid.placeNode<SubpassPipelineNode>(this, resource).get();
        else if (resource->getType() == "ImageNodeResource")
            newNode = m_grid.placeNode<ImageNode>(this, resource).get();

        if (!newNode) continue;

        newNode->setPos({ resource->getPos().x, resource->getPos().y });
        newNode->setUID(resource->getNodeID());
        newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
    }

    for (int i = 0; i < m_selectedPassMeta->getConnections().size(); ++i)
    {
        GraphResource::Connection* connection = m_selectedPassMeta->getConnections()[i];
        const size_t leftUID = connection->getFirst()->getFirst();
        const int leftPin = connection->getFirst()->getSecond();
        const size_t rightUID = connection->getSecond()->getFirst();
        const int rightPin = connection->getSecond()->getSecond();
        ImFlow::Pin* left = m_grid.getNodes().at(leftUID)->outPinByFilderID(leftPin);
        ImFlow::Pin* right = m_grid.getNodes().at(rightUID)->inPinByFilderID(rightPin);
        left->createLink(right);
    }
}
