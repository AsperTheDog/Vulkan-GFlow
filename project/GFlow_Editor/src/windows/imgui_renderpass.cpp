#include "imgui_renderpass.hpp"

#include "editor.hpp"
#include "imgui.h"

#include "resource_manager.hpp"
#include "resources/renderpass.hpp"

#include "metaresources/graph.hpp"
#include "metaresources/renderpass.hpp"

#include "nodes/renderpass_nodes.hpp"

class NodeCreateHelper
{
public:
    template <typename T, typename Tr>
    static std::shared_ptr<T> createNode(ImGuiRenderPassWindow* window)
    {
        Tr* resource = window->m_selectedPassMeta->addNode<Tr>();
        const std::shared_ptr<T> newNode = window->m_grid.placeNode<T>(window, resource);
        resource->setPos({ newNode->getPos().x, newNode->getPos().y });
        resource->setNodeID(newNode->getUID());
        newNode->getDestroyedSignal().connect(window, &ImGuiRenderPassWindow::onNodeDestroyed);
        return newNode;
    }

    template <typename T, typename Tr>
    static std::shared_ptr<T> createNodeAtPlace(ImGuiRenderPassWindow* window, const ImVec2& pos)
    {
        Tr* resource = window->m_selectedPassMeta->addNode<Tr>();
        const std::shared_ptr<T> newNode = window->m_grid.addNode<T>(pos, window, resource);
        resource->setPos({ newNode->getPos().x, newNode->getPos().y });
        resource->setNodeID(newNode->getUID());
        newNode->getDestroyedSignal().connect(window, &ImGuiRenderPassWindow::onNodeDestroyed);
        return newNode;
    }
};

ImGuiRenderPassWindow::ImGuiRenderPassWindow(const std::string_view& name, const bool defaultOpen) : ImGuiGraphWindow(name, defaultOpen)
{
    m_refreshRequestedSignal.connect(this, &ImGuiRenderPassWindow::recreateParserData);
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
            m_selectedPassMeta = gflow::parser::ResourceManager::createResource<RenderpassResource>(metaPath, nullptr);
            const std::shared_ptr<InitRenderpassNode> newNode = NodeCreateHelper::createNodeAtPlace<InitRenderpassNode, InitNodeResource>(this, {20, 20});
            m_selectedPassMeta->serialize();
            loadRenderPass(false);
        }
    }
}

void ImGuiRenderPassWindow::recreateParserData()
{
    if (m_selectedPass == nullptr) return;
    m_selectedPass->clearSubpasses();
    GFlowNode* next = getInit()->getNext();
    SubpassNode* subpass = nullptr;
    gflow::parser::RenderPassSubpass* subpassResource = nullptr;
    while (next != nullptr)
    {
        if (SubpassNode* subpassNode = dynamic_cast<SubpassNode*>(next))
        {
            if (subpass != nullptr)
                processSubpassConnections(subpass, subpassResource);
            subpass = subpassNode;
            subpassResource = m_selectedPass->addSubpass();
            next = subpassNode->getNext();
        }
        else if (SubpassPipelineNode* pipelineNode = dynamic_cast<SubpassPipelineNode*>(next))
        {
            PipelineNodeResource* pipelineResource = dynamic_cast<PipelineNodeResource*>(pipelineNode->getLinkedResource());
            if (pipelineResource->getPipeline() != nullptr)
            {
                gflow::parser::RenderPassPipeline* pipeline = subpassResource->addPipeline();
                pipeline->setPipeline(pipelineResource->getPipeline());
                processPipelineConnections(pipelineNode, pipelineResource);
            }
            next = pipelineNode->getNext();
        }
    }
    if (subpass != nullptr)
        processSubpassConnections(subpass, subpassResource);
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
                    NodeCreateHelper::createNode<SubpassNode, SubpassNodeResource>(this);
                if (ImGui::MenuItem("Pipeline"))
                    NodeCreateHelper::createNode<SubpassPipelineNode, PipelineNodeResource>(this);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Attachment"))
                    NodeCreateHelper::createNode<ImageNode, ImageNodeResource>(this);
                if (ImGui::MenuItem("Push Constant"))
                    NodeCreateHelper::createNode<PushConstantNode, PushConstantNodeResource>(this);
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

    m_selectedPassMeta->clearConnections();
    for (const std::weak_ptr<ImFlow::Link>& connection : m_grid.getLinks())
    {
        const ImFlow::Link* link = connection.lock().get();
        const size_t leftUID = link->left()->getParent()->getUID();
        const size_t leftPin = link->left()->getUID();
        const size_t rightUID = link->right()->getParent()->getUID();
        const size_t rightPin = link->right()->getUID();
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
        else if (resource->getType() == "PushConstantNodeResource")
            newNode = m_grid.placeNode<PushConstantNode>(this, resource).get();

        if (!newNode) continue;

        newNode->setPos({ resource->getPos().x, resource->getPos().y });
        newNode->setUID(resource->getNodeID());
        newNode->getDestroyedSignal().connect(this, &ImGuiRenderPassWindow::onNodeDestroyed);
    }

    for (int i = 0; i < m_selectedPassMeta->getConnections().size(); ++i)
    {
        const Connection* connection = m_selectedPassMeta->getConnections()[i];
        ImFlow::Pin* left = m_grid.getNodes().at(connection->getLeftUID())->outPin(connection->getLeftPin());
        ImFlow::Pin* right = m_grid.getNodes().at(connection->getRightUID())->inPin(connection->getRightPin());
        left->createLink(right);
    }
}

void ImGuiRenderPassWindow::processSubpassConnections(SubpassNode* subpass, gflow::parser::RenderPassSubpass* subpassResource) const
{
    const std::unordered_set<std::string> oldColorAttachments = subpass->getColorAttachments();
    const std::unordered_set<std::string> oldInputAttachments = subpass->getInputAttachments();

    std::unordered_set<std::string> colorAttachments;
    std::unordered_set<std::string> inputAttachments;
    bool hasDepth = false;
    for (gflow::parser::RenderPassPipeline* pipelineResource : subpassResource->getPipelines())
    {
        VulkanShader::ReflectionManager* data = pipelineResource->getPipeline()->getShaderReflectionData(gflow::parser::Pipeline::VERTEX);
        if (data->isValid())
        {
            for (const spirv_cross::Resource& resource : data->getResources().subpass_inputs)
                inputAttachments.insert(data->getName(resource.id, resource.name));
        }
        data = pipelineResource->getPipeline()->getShaderReflectionData(gflow::parser::Pipeline::FRAGMENT);
        if (data->isValid())
        {
            for (const spirv_cross::Resource& resource : data->getResources().stage_outputs)
                colorAttachments.insert(data->getName(resource.id, resource.name));
            for (const spirv_cross::Resource& resource : data->getResources().subpass_inputs)
                inputAttachments.insert(data->getName(resource.id, resource.name));
        }

        hasDepth |= subpassResource->hasDepthAttachment();
    }

    for (const std::string& attachment : oldColorAttachments)
        if (!colorAttachments.contains(attachment))
            subpass->removeColorAttachmentPin(attachment);
    for (const std::string& attachment : colorAttachments)
        if (!oldColorAttachments.contains(attachment))
            subpass->addColorAttachmentPin(attachment, true);

    for (const std::string& attachment : oldInputAttachments)
        if (!inputAttachments.contains(attachment))
            subpass->removeInputAttachmentPin(attachment);
    for (const std::string& attachment : inputAttachments)
        if (!oldInputAttachments.contains(attachment))
            subpass->addInputAttachmentPin(attachment, true);

    subpass->setDepthAttachment(hasDepth, false);
}

void ImGuiRenderPassWindow::processPipelineConnections(SubpassPipelineNode* pipeline, PipelineNodeResource* pipelineResource) const
{
    const std::unordered_set<std::string> oldPushConstants = pipeline->getPushConstants();

    std::unordered_set<std::string> pushConstants;
    const VulkanShader::ReflectionManager* data = pipelineResource->getPipeline()->getShaderReflectionData(gflow::parser::Pipeline::VERTEX);
    if (data->isValid())
    {
        for (const spirv_cross::Resource& resource : data->getResources().push_constant_buffers)
            pushConstants.insert(data->getName(resource.id, resource.name));
    }
    data = pipelineResource->getPipeline()->getShaderReflectionData(gflow::parser::Pipeline::FRAGMENT);
    if (data->isValid())
    {
        for (const spirv_cross::Resource& resource : data->getResources().push_constant_buffers)
            pushConstants.insert(data->getName(resource.id, resource.name));
    }

    for (const std::string& pushConstant : oldPushConstants)
        if (!pushConstants.contains(pushConstant))
            pipeline->removePushConstantPin(pushConstant);
    for (const std::string& pushConstant : pushConstants)
        if (!oldPushConstants.contains(pushConstant))
            pipeline->addPushConstantPin(pushConstant, true);
}

InitRenderpassNode* ImGuiRenderPassWindow::getInit()
{
    for (const std::shared_ptr<ImFlow::BaseNode>& val : m_grid.getNodes() | std::views::values)
    {
        if (InitRenderpassNode* node = dynamic_cast<InitRenderpassNode*>(val.get()))
            return node;
    }
    return nullptr;
}
