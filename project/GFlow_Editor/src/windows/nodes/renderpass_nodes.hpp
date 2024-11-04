#pragma once
#include "windows/nodes/base_node.hpp"


class InitNodeResource;
class SubpassNodeResource;
class PipelineNodeResource;
class ImageNodeResource;

enum RenderPassPinType : uint8_t
{
    INIT,
    SUBPASS,
    PIPELINE,
    IMAGE
};

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

private:
    SubpassNodeResource* m_resource = nullptr;
    
    std::shared_ptr<ImFlow::InPin<int>> m_in;
    std::shared_ptr<ImFlow::OutPin<int>> m_out;
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