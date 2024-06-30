#include "subpass_node.hpp"

bool SubpassNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
	return variable != "subpassID";
}

SubpassNode::SubpassNode(ImGuiGraphWindow* parent) : GFlowNode("Subpass", parent)
{
	m_mainIn.addStaticPins();
	m_mainOut.addStaticPins();
}
