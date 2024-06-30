#pragma once
#include "renderpass_node_pins.hpp"
#include "windows/nodes/base_node.hpp"

class InitRenderpassNode final : public GFlowNode
{
public:
    explicit InitRenderpassNode(ImGuiGraphWindow* parent) : GFlowNode("START", parent)
    {
	    m_out.addStaticPins();
    }

    void destroy() override {}

    gflow::parser::Resource* getLinkedResource() override { return nullptr; }

private:
    InitPassOutputPin m_out{this, "Subpass"};
};

