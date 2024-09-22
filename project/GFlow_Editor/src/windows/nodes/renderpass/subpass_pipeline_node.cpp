#include "subpass_pipeline_node.hpp"

SubpassPipelineNode::SubpassPipelineNode(ImGuiGraphWindow* parent, gflow::parser::Resource* resource)
    : GFlowNode("Pipeline", parent), m_resource(dynamic_cast<PipelineNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(109,0,181,255), ImColor(233,241,244,255), 3.5f));
	m_mainIn.addStaticPins();
	m_mainOut.addStaticPins();
}
