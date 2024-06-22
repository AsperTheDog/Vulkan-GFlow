#pragma once
#include "windows/nodes/base_node.hpp"

class InitRenderpassNode final : public GFlowNode
{
public:
    InitRenderpassNode() : GFlowNode("Init Renderpass") {}

    void destroy() override {}

    gflow::parser::Resource* getLinkedResource() override { return nullptr; }
};

