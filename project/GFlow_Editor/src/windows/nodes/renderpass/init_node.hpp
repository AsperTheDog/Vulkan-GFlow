#pragma once
#include "renderpass_node_pins.hpp"
#include "windows/nodes/base_node.hpp"

class InitRenderpassNode final : public GFlowNode
{
public:
    explicit InitRenderpassNode(ImGuiGraphWindow* parent) : GFlowNode("START", parent)
    {
        setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(71,142,173,255), ImColor(233,241,244,255), 3.5f));
	    m_out.addStaticPins();
    }

    void destroy() override {}

    gflow::parser::Resource* getLinkedResource() override { return nullptr; }

private:
    InitPassOutputPin m_out{this, ""};
};

