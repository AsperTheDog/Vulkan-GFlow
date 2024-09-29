#pragma once
#include "metaresources/Renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class ImageNode final : public GFlowNode
{
public:
    ImageNode(ImGuiGraphWindow* parent, NodeResource* resource);

    NodeResource* getLinkedResource() override { return m_resource; }
    void onResourceUpdated(const gflow::parser::ResourceElemPath& element) override;

private:
    ImageNodeResource* m_resource = nullptr;
};