#pragma once
#include "renderpass_node_pins.hpp"
#include "windows/nodes/base_node.hpp"

class SubpassPipelineNodeResource final : public gflow::parser::Resource
{
public:

	DECLARE_RESOURCE(SubpassPipelineNodeResource)
};

class SubpassPipelineNode final : public GFlowNode
{
public:
	SubpassPipelineNode(ImGuiGraphWindow* parent);

	gflow::parser::Resource* getLinkedResource() override { return &m_resource; }

private:
	SubpassPipelineNodeResource m_resource{""};

	PipelineInputPin m_mainIn{this, ""};
	PipelineOutputPin m_mainOut{this, ""};
};

