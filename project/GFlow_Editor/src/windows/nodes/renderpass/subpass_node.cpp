#include "subpass_node.hpp"

gflow::parser::DataUsage SubpassNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
	return variable != "subpassID" ? gflow::parser::USED : gflow::parser::NOT_USED;
}

SubpassNode::SubpassNode(ImGuiGraphWindow* parent) : GFlowNode("Subpass", parent)
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,60,0,255), ImColor(233,241,244,255), 3.5f));
	m_mainIn.addStaticPins();
	m_mainOut.addStaticPins();
}
