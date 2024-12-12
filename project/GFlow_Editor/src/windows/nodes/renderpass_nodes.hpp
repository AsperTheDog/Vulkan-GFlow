#pragma once
#include <unordered_set>

#include "metaresources/renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class ImageAttachmentNode final : public GFlowNode
{
public:
    ImageAttachmentNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

private:
    ImageAttachmentNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class PushConstantNode final : public GFlowNode
{
public:
    PushConstantNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

private:
    PushConstantNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class SubpassPipelineNode final : public GFlowNode
{
public:
    explicit SubpassPipelineNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;
    
    void addPushConstantPin(const std::string& name, bool addToResource);
    void removePushConstantPin(const std::string& name);
    std::vector<std::string> getColorAttachmentPins();

    [[nodiscard]] std::unordered_set<std::string> getPushConstants() const;

private:
	PipelineNodeResource* m_resource = nullptr;
    
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;

    std::vector<std::shared_ptr<ImFlow::InPin<int>>> m_PushConstantPins;
};

class SubpassNode final : public GFlowNode
{
public:
    explicit SubpassNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    [[nodiscard]] GFlowNode* getNext() const;

    void addColorAttachmentPin(const std::string& name, bool addToResource);
    void removeColorAttachmentPin(const std::string& name);
    void addInputAttachmentPin(const std::string& name, bool addToResource);
    void removeInputAttachmentPin(const std::string& name);
    void removeAllReflectionPins();

    void setDepthAttachment(bool enabled, bool force);
    
    [[nodiscard]] std::unordered_set<std::string> getColorAttachments() const;
    [[nodiscard]] std::unordered_set<std::string> getInputAttachments() const;


private:
    SubpassNodeResource* m_resource = nullptr;
    
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;

    std::vector<std::shared_ptr<ImFlow::InPin<int>>> m_ColorAttachmentPins;
    std::vector<std::shared_ptr<ImFlow::InPin<int>>> m_InputAttachmentPins;
    std::shared_ptr<ImFlow::InPin<int>> m_DepthAttachmentPin;
};

class InitRenderpassNode final : public GFlowNode
{
public:
    explicit InitRenderpassNode(ImGuiGraphWindow* parent, NodeResource* resource);

    void destroy() override {}

    NodeResource* getLinkedResource() override { return m_resource; }

    [[nodiscard]] GFlowNode* getNext() const;

private:
    InitNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};