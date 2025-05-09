#include "imgui_execution.hpp"

#include "editor.hpp"
#include "imgui.h"
#include "nodes/execution_nodes.hpp"
#include "resources/project.hpp"

class NodeCreateHelper
{
public:
    template <typename T, typename Tr>
    static std::shared_ptr<T> createNode(ImGuiExecutionWindow* window)
    {
        Tr* resource = window->m_selectedExecMeta->addNode<Tr>();
        const std::shared_ptr<T> newNode = window->m_grid.placeNode<T>(window, resource);
        resource->setPos({ newNode->getPos().x, newNode->getPos().y });
        resource->setNodeID(newNode->getUID());
        newNode->getDestroyedSignal().connect(window, &ImGuiExecutionWindow::onNodeDestroyed);
        return newNode;
    }

    template <typename T, typename Tr>
    static std::shared_ptr<T> createNodeAtPlace(ImGuiExecutionWindow* window, const ImVec2& pos)
    {
        Tr* resource = window->m_selectedExecMeta->addNode<Tr>();
        const std::shared_ptr<T> newNode = window->m_grid.addNode<T>(pos, window, resource);
        resource->setPos({ newNode->getPos().x, newNode->getPos().y });
        resource->setNodeID(newNode->getUID());
        newNode->getDestroyedSignal().connect(window, &ImGuiExecutionWindow::onNodeDestroyed);
        return newNode;
    }
};

ImGuiExecutionWindow::ImGuiExecutionWindow(const std::string& name, const bool defaultOpen)
: ImGuiGraphWindow(name, defaultOpen)
{
    m_refreshRequestedSignal.connect(this, &ImGuiExecutionWindow::buildProject);
}

void ImGuiExecutionWindow::buildProject()
{
    if (m_selectedExecMeta == nullptr) return;
    Editor::getCurrentProject()->clear();
    GFlowNode* next = getInit()->getNext();
    gflow::parser::ProjectRenderpassSubpass* subpassResource = nullptr;
    gflow::parser::ProjectRenderpass* renderpassResource = nullptr;
    while (next != nullptr)
    {
        if (BeginExecutionNode* beginNode = dynamic_cast<BeginExecutionNode*>(next))
        {
            BeginExecutionNodeResource* beginResource = dynamic_cast<BeginExecutionNodeResource*>(beginNode->getLinkedResource());
            renderpassResource = Editor::getCurrentProject()->addRenderpass();
            renderpassResource->setRenderpass(beginResource->getRenderpass());
            subpassResource = renderpassResource->addSubpass();
            processRenderpassConnections(beginNode, renderpassResource);
            next = beginNode->getNext();
        }
        else if (const NextExecutionNode* nextNode = dynamic_cast<NextExecutionNode*>(next))
        {
            subpassResource = renderpassResource->addSubpass();
            next = nextNode->getNext();
        }
        else if (const EndExecutionNode* endNode = dynamic_cast<EndExecutionNode*>(next))
        {
            renderpassResource = nullptr;
            next = endNode->getNext();
        }
        else if (BindPushConstantNode* bindNode = dynamic_cast<BindPushConstantNode*>(next))
        {
            processBindPushConstantConnections(bindNode, renderpassResource);
            next = bindNode->getNext();
        }
        else if (DrawCallNode* drawNode = dynamic_cast<DrawCallNode*>(next))
        {
            gflow::parser::ProjectRenderpassDrawCall* drawCallResource = subpassResource->addDrawCall();
            DrawCallNodeResource* drawCallNodeResource = dynamic_cast<DrawCallNodeResource*>(drawNode->getLinkedResource());
            drawCallResource->setPipeline(drawCallNodeResource->getPipeline());
            processDrawCallConnections(drawNode, drawCallResource);
            next = drawNode->getNext();
        }
    }
}

void ImGuiExecutionWindow::onProjectLoaded()
{
    if (m_selectedExecMeta != nullptr)
        saveExecution();

    if (m_sidePanelTarget != nullptr)
    {
        m_sidePanelTarget->setInspectionStatus(false);
        m_sidePanelTarget = nullptr;
    }
    clearGrid();
    const std::string metaPath = Editor::getCurrentProject()->getMetaPath();
    if (gflow::parser::ResourceManager::hasResource(metaPath))
    {
        m_selectedExecMeta = dynamic_cast<ExecutionResource*>(gflow::parser::ResourceManager::getResource(metaPath));
        loadExecution(true);
    }
    else
    {
        m_selectedExecMeta = gflow::parser::ResourceManager::createResource<ExecutionResource>(metaPath, nullptr);
        const std::shared_ptr<InitExecutionNode> newNode = NodeCreateHelper::createNodeAtPlace<InitExecutionNode, InitNodeResource>(this, {20, 20});
        m_selectedExecMeta->serialize();
        loadExecution(false);
    }
    loadExecution();
}

void ImGuiExecutionWindow::save()
{
    saveExecution();
}

void ImGuiExecutionWindow::onNodeDestroyed(GFlowNode* node)
{
    m_selectedExecMeta->removeNode(node);
    if (node == m_sidePanelTarget)
    {
        m_sidePanelTarget->setInspectionStatus(false);
        m_sidePanelTarget = nullptr;
    }
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
                    NodeCreateHelper::createNode<BeginExecutionNode, BeginExecutionNodeResource>(this);
                if (ImGui::MenuItem("Next"))
                    NodeCreateHelper::createNode<NextExecutionNode, NextExecutionNodeResource>(this);
                if (ImGui::MenuItem("End"))
                    NodeCreateHelper::createNode<EndExecutionNode, EndExecutionNodeResource>(this);
                if (ImGui::MenuItem("Draw Call"))
                    NodeCreateHelper::createNode<DrawCallNode, DrawCallNodeResource>(this);
                if (ImGui::MenuItem("Bind Push Constant"))
                    NodeCreateHelper::createNode<BindPushConstantNode, BindPushConstantNodeResource>(this);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Resources"))
            {
                if (ImGui::MenuItem("Image"))
                    NodeCreateHelper::createNode<ImageNode, ImageNodeResource>(this);
                if (ImGui::MenuItem("Model"))
                    NodeCreateHelper::createNode<ModelNode, ModelNodeResource>(this);
                if (ImGui::MenuItem("Data Decompose"))
                    NodeCreateHelper::createNode<DataDecomposeNode, DataDecomposeNodeResource>(this);
                if (ImGui::MenuItem("External Argument"))
                    NodeCreateHelper::createNode<ExternalArgumentNode, ExternalArgumentNodeResource>(this);
                if (ImGui::BeginMenu("Camera"))
                {
                    if (ImGui::MenuItem("Flight"))
                        NodeCreateHelper::createNode<CameraFlightNode, CameraNodeResource>(this);
                    if (ImGui::MenuItem("Object"))
                        NodeCreateHelper::createNode<CameraObjectNode, ObjectCameraNodeResource>(this);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Primitives"))
                {
                    if (ImGui::BeginMenu("Data Types"))
                    {
                        if (ImGui::MenuItem("Float"))
                            NodeCreateHelper::createNode<PrimitiveFloatNode, PrimitiveFloatNodeResource>(this);
                        if (ImGui::MenuItem("Int"))
                            NodeCreateHelper::createNode<PrimitiveIntNode, PrimitiveIntNodeResource>(this);
                        if (ImGui::MenuItem("Color"))
                            NodeCreateHelper::createNode<PrimitiveColorNode, PrimitiveColorNodeResource>(this);
                        if (ImGui::MenuItem("Vec2"))
                            NodeCreateHelper::createNode<PrimitiveVec2Node, PrimitiveVec2NodeResource>(this);
                        if (ImGui::MenuItem("Vec3"))
                            NodeCreateHelper::createNode<PrimitiveVec3Node, PrimitiveVec3NodeResource>(this);
                        if (ImGui::MenuItem("Vec4"))
                            NodeCreateHelper::createNode<PrimitiveVec4Node, PrimitiveVec4NodeResource>(this);
                        if (ImGui::MenuItem("Mat3"))
                            NodeCreateHelper::createNode<PrimitiveMat3Node, PrimitiveMat3NodeResource>(this);
                        if (ImGui::MenuItem("Mat4"))
                            NodeCreateHelper::createNode<PrimitiveMat4Node, PrimitiveMat4NodeResource>(this);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Watcher"))
                NodeCreateHelper::createNode<WatcherNode, WatcherNodeResource>(this);
            ImGui::EndMenu();
        }
    }
    else ImGuiGraphWindow::rightClick(node);
}

void ImGuiExecutionWindow::saveExecution()
{
    if (m_selectedExecMeta == nullptr) return;
    for (std::shared_ptr<ImFlow::BaseNode>& val : m_grid.getNodes() | std::views::values)
    {
        GFlowNode* gnode = dynamic_cast<GFlowNode*>(val.get());
        if (gnode == nullptr) continue;
        NodeResource* resource = gnode->getLinkedResource();
        if (resource == nullptr) continue;
        resource->setPos({ gnode->getPos().x, gnode->getPos().y });
    }

    m_selectedExecMeta->clearConnections();
    for (const std::weak_ptr<ImFlow::Link>& connection : m_grid.getLinks())
    {
        const ImFlow::Link* link = connection.lock().get();
        const size_t leftUID = link->left()->getParent()->getUID();
        const size_t leftPin = link->left()->getUID();
        const size_t rightUID = link->right()->getParent()->getUID();
        const size_t rightPin = link->right()->getUID();
        m_selectedExecMeta->addConnection(leftUID, leftPin, rightUID, rightPin);
    }
}

void ImGuiExecutionWindow::loadExecution(const bool loadInit)
{
    for (int i = 0; i < m_selectedExecMeta->getNodes().size(); ++i)
    {
        NodeResource* resource = m_selectedExecMeta->getNodes()[i];
        GFlowNode* newNode = nullptr;
        if (resource->getType() == "InitNodeResource" && loadInit)
            newNode = m_grid.placeNode<InitExecutionNode>(this, resource).get();
        else if (resource->getType() == "BeginExecutionNodeResource" && loadInit)
            newNode = m_grid.placeNode<BeginExecutionNode>(this, resource).get();
        else if (resource->getType() == "NextExecutionNodeResource")
            newNode = m_grid.placeNode<NextExecutionNode>(this, resource).get();
        else if (resource->getType() == "EndExecutionNodeResource")
            newNode = m_grid.placeNode<EndExecutionNode>(this, resource).get();
        else if (resource->getType() == "BindPushConstantNodeResource")
            newNode = m_grid.placeNode<BindPushConstantNode>(this, resource).get();
        else if (resource->getType() == "DrawCallNodeResource")
            newNode = m_grid.placeNode<DrawCallNode>(this, resource).get();
        else if (resource->getType() == "ImageNodeResource")
            newNode = m_grid.placeNode<ImageNode>(this, resource).get();
        else if (resource->getType() == "ModelNodeResource")
            newNode = m_grid.placeNode<ModelNode>(this, resource).get();
        else if (resource->getType() == "DataDecomposeNodeResource")
            newNode = m_grid.placeNode<DataDecomposeNode>(this, resource).get();
        else if (resource->getType() == "PrimitiveFloatNodeResource")
            newNode = m_grid.placeNode<PrimitiveFloatNode>(this, resource).get();
        else if (resource->getType() == "PrimitiveIntNodeResource")
            newNode = m_grid.placeNode<PrimitiveIntNode>(this, resource).get();
        else if (resource->getType() == "PrimitiveColorNodeResource")
            newNode = m_grid.placeNode<PrimitiveColorNode>(this, resource).get();
        else if (resource->getType() == "PrimitiveVec2NodeResource")
            newNode = m_grid.placeNode<PrimitiveVec2Node>(this, resource).get();
        else if (resource->getType() == "PrimitiveVec3NodeResource")
            newNode = m_grid.placeNode<PrimitiveVec3Node>(this, resource).get();
        else if (resource->getType() == "PrimitiveVec4NodeResource")
            newNode = m_grid.placeNode<PrimitiveVec4Node>(this, resource).get();
        else if (resource->getType() == "PrimitiveMat3NodeResource")
            newNode = m_grid.placeNode<PrimitiveMat3Node>(this, resource).get();
        else if (resource->getType() == "PrimitiveMat4NodeResource")
            newNode = m_grid.placeNode<PrimitiveMat4Node>(this, resource).get();
        else if (resource->getType() == "ExternalArgumentNodeResource")
            newNode = m_grid.placeNode<ExternalArgumentNode>(this, resource).get();
        else if (resource->getType() == "WatcherNodeResource")
            newNode = m_grid.placeNode<WatcherNode>(this, resource).get();
        else if (resource->getType() == "ObjectCameraNodeResource")
            newNode = m_grid.placeNode<CameraObjectNode>(this, resource).get();
        else if (resource->getType() == "CameraFlightNodeResource")
            newNode = m_grid.placeNode<CameraFlightNode>(this, resource).get();

        if (!newNode) continue;

        newNode->setPos({ resource->getPos().x, resource->getPos().y });
        newNode->setUID(resource->getNodeID());
        newNode->getDestroyedSignal().connect(this, &ImGuiExecutionWindow::onNodeDestroyed);
    }

    for (int i = 0; i < m_selectedExecMeta->getConnections().size(); ++i)
    {
        const Connection* connection = m_selectedExecMeta->getConnections()[i];
        ImFlow::Pin* left = m_grid.getNodes().at(connection->getLeftUID())->outPin(connection->getLeftPin());
        ImFlow::Pin* right = m_grid.getNodes().at(connection->getRightUID())->inPin(connection->getRightPin());
        left->createLink(right);
    }
}

void ImGuiExecutionWindow::processRenderpassConnections(BeginExecutionNode* renderpassNode, gflow::parser::ProjectRenderpass* renderpassResource) const
{
    if (renderpassResource->getRenderpass() == nullptr)
    {
        renderpassNode->removeAllReflectionPins();
        return;
    }

    const std::unordered_set<std::string> oldAttachments = renderpassNode->getAttachments();

    std::unordered_set<std::string> newAttachments{};
    for (const std::string& attachment : renderpassResource->getRenderpass()->getAttachmentIDs())
        newAttachments.insert(attachment);
    
    for (const std::string& attachment : oldAttachments)
        if (!newAttachments.contains(attachment))
            renderpassNode->removeAttachmentPin(attachment);
    for (const std::string& attachment : newAttachments)
        if (!oldAttachments.contains(attachment))
            renderpassNode->addAttachmentPin(attachment, true);
}

void ImGuiExecutionWindow::processDrawCallConnections(DrawCallNode* drawNode, gflow::parser::ProjectRenderpassDrawCall* drawCallResource) const
{
    if (drawCallResource->getPipeline() == nullptr)
    {
        drawNode->setModelPin(false, false);
        return;
    }

    gflow::parser::Pipeline* pipeline = drawCallResource->getPipeline();
    pipeline->getShaderReflectionData(gflow::parser::Pipeline::VERTEX);
    /*if (reflectionData == nullptr)
    {
        drawNode->setModelPin(false, false);
        return;
    }*/

    // Check if the vertex shader has vertex inputs
}

void ImGuiExecutionWindow::processBindPushConstantConnections(BindPushConstantNode* bindNode, gflow::parser::ProjectRenderpass* renderpassResource) const
{
    if (renderpassResource == nullptr || renderpassResource->getRenderpass() == nullptr)
        return;

    if (bindNode->getPushConstantDataPin()->getLink().expired() || bindNode->getPushConstantDataPin()->getLink().lock()->left()->getParent() == nullptr)
        return;

    gflow::parser::RenderPass* renderpass = renderpassResource->getRenderpass();
    const std::string structID = bindNode->getLinkedResource()->getValue<std::string>("structID");
    std::vector<std::string> pushConstants = renderpass->getPushConstantIDs(false);
    DataDecomposeNode* pushConstantNode = dynamic_cast<DataDecomposeNode*>(bindNode->getPushConstantDataPin()->getLink().lock()->left()->getParent());

    if (std::ranges::find(pushConstants, structID) == pushConstants.end())
    {
        pushConstantNode->removeAllReflectionPins();
        return;
    }

    std::unordered_set<std::string> oldPins = pushConstantNode->getComponents();
    std::vector<std::string> newPins = renderpass->getPushConstantStructure(structID)->getElementNames();

    for (const std::string& pin : oldPins)
        if (std::ranges::find(newPins, pin) == newPins.end())
            pushConstantNode->removeComponentPin(pin);

    for (const std::string& pin : newPins)
        if (std::ranges::find(oldPins, pin) == oldPins.end())
            pushConstantNode->addComponentPin(pin, true);
}

InitExecutionNode* ImGuiExecutionWindow::getInit()
{
    for (const std::shared_ptr<ImFlow::BaseNode>& node : m_grid.getNodes() | std::views::values)
    {
        if (InitExecutionNode* init = dynamic_cast<InitExecutionNode*>(node.get()))
            return init;
    }
    return nullptr;
}
