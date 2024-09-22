#pragma once
#include "renderpass_node_pins.hpp"
#include "metaresources/Renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class SubpassPipelineNode final : public GFlowNode
{
public:
    explicit SubpassPipelineNode(ImGuiGraphWindow* parent, gflow::parser::Resource* resource);

	gflow::parser::Resource* getLinkedResource() override { return m_resource; }

private:
	PipelineNodeResource* m_resource = nullptr;

	PipelineInputPin m_mainIn{this, ""};
	PipelineOutputPin m_mainOut{this, ""};
};

