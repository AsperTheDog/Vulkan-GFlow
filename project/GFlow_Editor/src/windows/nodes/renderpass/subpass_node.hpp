#pragma once
#include "renderpass_node_pins.hpp"
#include "windows/nodes/base_node.hpp"

class SubpassNodeResource final : public gflow::parser::Resource
{
    EXPORT(int, subpassID);

    void initContext(ExportData* metadata) override {}

    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_RESOURCE(SubpassNodeResource)
};

class SubpassNode final : public GFlowNode
{
public:
    explicit SubpassNode(ImGuiGraphWindow* parent);

    gflow::parser::Resource* getLinkedResource() override { return &m_resource; }

private:
    SubpassNodeResource m_resource{""};

    SubpassInputPin m_mainIn{this, ""};
    SubpassOutputPin m_mainOut{this, ""};
};

