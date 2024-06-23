#pragma once
#include "windows/nodes/base_node.hpp"

class SubpassNodeResource final : public gflow::parser::Resource
{
    EXPORT(int, example);

    void initContext(ExportData* metadata) override {}
public:
    DECLARE_RESOURCE(SubpassNodeResource)
};

class SubpassNode final : public GFlowNode
{
public:
    SubpassNode() : GFlowNode("Subpass") {}

    gflow::parser::Resource* getLinkedResource() override { return &m_resource; }

private:
    SubpassNodeResource m_resource{""};
};

