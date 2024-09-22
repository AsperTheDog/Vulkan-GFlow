#include "init_node.hpp"

#include "metaresources/Renderpass.hpp"

InitRenderpassNode::InitRenderpassNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("START", parent), m_resource(dynamic_cast<InitNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(71,142,173,255), ImColor(233,241,244,255), 3.5f));
	m_out.addStaticPins();
}
