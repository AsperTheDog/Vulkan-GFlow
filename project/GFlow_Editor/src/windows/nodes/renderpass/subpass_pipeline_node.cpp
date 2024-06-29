#include "subpass_pipeline_node.hpp"

SubpassPipelineNode::SubpassPipelineNode(ImGuiGraphWindow* parent) : GFlowNode("Pipeline", parent)
{
	m_mainIn.addStaticPins();
	m_mainOut.addStaticPins();
}
