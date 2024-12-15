#pragma once

#include "metaresources/execution.hpp"
#include "windows/nodes/base_node.hpp"

class InitExecutionNode final : public GFlowNode
{
public:
    explicit InitExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource);

    void destroy() override {}

    NodeResource* getLinkedResource() override { return m_resource; }

    [[nodiscard]] GFlowNode* getNext() const;

private:
    InitNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class BeginExecutionNode final : public GFlowNode
{
    
public:
    explicit BeginExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;

    void addAttachmentPin(const std::string& name, bool addToResource);
    void removeAttachmentPin(const std::string& name);
    void removeAllReflectionPins();
    
    [[nodiscard]] std::unordered_set<std::string> getAttachments() const;

private:
    BeginExecutionNodeResource* m_resource = nullptr;
    
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;

    std::vector<std::shared_ptr<ImFlow::InPin<int>>> m_AttachmentPins;
};

class NextExecutionNode final : public GFlowNode
{
public:
    explicit NextExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;

private:
    NextExecutionNodeResource* m_resource = nullptr;
    
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class EndExecutionNode final : public GFlowNode
{
public:
    explicit EndExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;

private:
    EndExecutionNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class BindPushConstantNode final : public GFlowNode
{
public:
    explicit BindPushConstantNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

    std::shared_ptr<ImFlow::InPin<int>> getPushConstantDataPin() const { return m_pushConstantData; }

private:
    BindPushConstantNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;

    std::shared_ptr<ImFlow::InPin<int>> m_pushConstantData;
};

class DrawCallNode final : public GFlowNode
{
public:
    explicit DrawCallNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;

    void setModelPin(bool enabled, bool force);

private:
    DrawCallNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
    std::shared_ptr<ImFlow::InPin<int>> m_modelPin;
};

class ImageNode final : public GFlowNode
{
public:
    explicit ImageNode(ImGuiGraphWindow* parent, NodeResource* resource);
    NodeResource* getLinkedResource() override { return m_resource; }
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

private:
    ImageNodeResource* m_resource = nullptr;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class ModelNode final : public GFlowNode
{
public:
    explicit ModelNode(ImGuiGraphWindow* parent, NodeResource* resource);
    NodeResource* getLinkedResource() override { return m_resource; }

private:
    ModelNodeResource* m_resource = nullptr;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class DataDecomposeNode final : public GFlowNode
{
public:
    explicit DataDecomposeNode(ImGuiGraphWindow* parent, NodeResource* resource);
    NodeResource* getLinkedResource() override { return m_resource; }

    void addComponentPin(const std::string& name, bool addToResource);
    void removeComponentPin(const std::string& name);
    void removeAllReflectionPins();
    
    [[nodiscard]] std::unordered_set<std::string> getComponents() const;

private:
    DataDecomposeNodeResource* m_resource = nullptr;

    std::vector<std::shared_ptr<ImFlow::InPin<int>>> m_ins;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class ExternalArgumentNode final : public GFlowNode
{
public:
    explicit ExternalArgumentNode(ImGuiGraphWindow* parent, NodeResource* resource);
    NodeResource* getLinkedResource() override { return m_resource; }
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

private:
    ExternalArgumentNodeResource* m_resource = nullptr;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class CameraFlightNode final : public GFlowNode
{
public:
    explicit CameraFlightNode(ImGuiGraphWindow* parent, NodeResource* resource);
    NodeResource* getLinkedResource() override { return m_resource; }

private:
    CameraNodeResource* m_resource = nullptr;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class CameraObjectNode final : public GFlowNode
{
public:
    explicit CameraObjectNode(ImGuiGraphWindow* parent, NodeResource* resource);
    NodeResource* getLinkedResource() override { return m_resource; }

private:
    ObjectCameraNodeResource* m_resource = nullptr;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class WatcherNode final : public GFlowNode
{
public:
    explicit WatcherNode(ImGuiGraphWindow* parent, NodeResource* resource);
    [[nodiscard]] GFlowNode* getNext() const;

    NodeResource* getLinkedResource() override { return m_resource; }


private:
    WatcherNodeResource* m_resource = nullptr;
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::InPin<int>> m_Image;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

template<typename T>
class PrimitiveNode final : public GFlowNode
{
public:
    explicit PrimitiveNode(ImGuiGraphWindow* parent, NodeResource* resource) : GFlowNode(T::getNodeName(), parent)
    {
        setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(26,102,120,255), ImColor(233,241,244,255), 3.5f));
        m_resource = dynamic_cast<T*>(resource);

        static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(26,102,120,255), 4, 4.f, 4.67f, 4.2f, 1.3f));
        m_out = addOUT<int>("-->", pinColor);
        m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
        m_out->setFilterID(PRIMITIVE);
    }

    NodeResource* getLinkedResource() override { return m_resource; }

private:
    T* m_resource = nullptr;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

typedef PrimitiveNode<PrimitiveFloatNodeResource> PrimitiveFloatNode;
typedef PrimitiveNode<PrimitiveIntNodeResource> PrimitiveIntNode;
typedef PrimitiveNode<PrimitiveColorNodeResource> PrimitiveColorNode;
typedef PrimitiveNode<PrimitiveVec2NodeResource> PrimitiveVec2Node;
typedef PrimitiveNode<PrimitiveVec3NodeResource> PrimitiveVec3Node;
typedef PrimitiveNode<PrimitiveVec4NodeResource> PrimitiveVec4Node;
typedef PrimitiveNode<PrimitiveMat3NodeResource> PrimitiveMat3Node;
typedef PrimitiveNode<PrimitiveMat4NodeResource> PrimitiveMat4Node;