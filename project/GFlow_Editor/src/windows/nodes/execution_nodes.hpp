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

private:
    DrawCallNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};