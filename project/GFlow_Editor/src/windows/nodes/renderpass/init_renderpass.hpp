#pragma once
#include "windows/nodes/base_node.hpp"

class InitRenderpassNode final : public GFlowNode
{
public:
    explicit InitRenderpassNode(ImGuiGraphWindow* parent) : GFlowNode("START", parent) {}

    void destroy() override {}

    gflow::parser::Resource* getLinkedResource() override { return nullptr; }
};

