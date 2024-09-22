#pragma once
#include "renderpass_node_pins.hpp"
#include "windows/nodes/base_node.hpp"

class InitNodeResource;

class InitRenderpassNode final : public GFlowNode
{
public:
    explicit InitRenderpassNode(ImGuiGraphWindow* parent, NodeResource* resource);

    void destroy() override {}

    NodeResource* getLinkedResource() override { return nullptr; }

private:
    InitNodeResource* m_resource = nullptr;
    InitPassOutputPin m_out{this, ""};
};

