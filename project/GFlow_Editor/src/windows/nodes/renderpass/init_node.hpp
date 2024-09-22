#pragma once
#include "renderpass_node_pins.hpp"
#include "metaresources/Renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class InitRenderpassNode final : public GFlowNode
{
public:
    explicit InitRenderpassNode(ImGuiGraphWindow* parent, NodeResource* resource);

    void destroy() override {}

    NodeResource* getLinkedResource() override { return m_resource; }

private:
    InitNodeResource* m_resource = nullptr;
    InitPassOutputPin m_out{this, ""};
};

