#pragma once
#include <unordered_set>

#include "metaresources/renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class ImageNode final : public GFlowNode
{
public:
    ImageNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override;
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

private:
    ImageNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class SubpassPipelineNode final : public GFlowNode
{
public:
    explicit SubpassPipelineNode(ImGuiGraphWindow* parent, NodeResource* resource);

	NodeResource* getLinkedResource() override;
    [[nodiscard]] GFlowNode* getNext() const;

    std::vector<std::string> getColorAttachmentPins();

private:
	PipelineNodeResource* m_resource = nullptr;
    
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

class SubpassNode final : public GFlowNode
{
public:
    explicit SubpassNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override;
    [[nodiscard]] GFlowNode* getNext() const;

    void addColorAttachmentPin(const std::string& name, bool addToResource);
    void removeColorAttachmentPin(const std::string& name);
    void addInputAttachmentPin(const std::string& name, bool addToResource);
    void removeInputAttachmentPin(const std::string& name);
    void removeAllAttachmentPins();

    void setDepthAttachment(bool enabled);

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

    NodeResource* getLinkedResource() override;

    [[nodiscard]] GFlowNode* getNext() const;

private:
    InitNodeResource* m_resource = nullptr;

    std::shared_ptr<ImFlow::OutPin<int>> m_out;
};

template <typename... Ints>
std::function<bool(const ImFlow::Pin*, const ImFlow::Pin*)> getLambdaFilter(Ints... filterIDs) {
    return [filterIDs...](const ImFlow::Pin* pin1, const ImFlow::Pin* pin2) -> bool { 
        int id = pin1->getFilterID();
        return ((id == filterIDs) || ...);  // Fold expression to match any filterID
    };
}