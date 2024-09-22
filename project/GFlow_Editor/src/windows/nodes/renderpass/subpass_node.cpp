#include "subpass_node.hpp"

SubpassNode::SubpassNode(ImGuiGraphWindow* parent, gflow::parser::Resource* resource)
    : GFlowNode("Subpass", parent), m_resource(dynamic_cast<SubpassNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,60,0,255), ImColor(233,241,244,255), 3.5f));
	m_mainIn.addStaticPins();
	m_mainOut.addStaticPins();
}
