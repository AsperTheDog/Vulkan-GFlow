#pragma once
#include "renderpass_node_pins.hpp"
#include "metaresources/Renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class SubpassNode final : public GFlowNode
{
public:
    explicit SubpassNode(ImGuiGraphWindow* parent, gflow::parser::Resource* resource);

    gflow::parser::Resource* getLinkedResource() override { return m_resource; }

private:
    SubpassNodeResource* m_resource = nullptr;

    SubpassInputPin m_mainIn{this, ""};
    SubpassOutputPin m_mainOut{this, ""};
};

