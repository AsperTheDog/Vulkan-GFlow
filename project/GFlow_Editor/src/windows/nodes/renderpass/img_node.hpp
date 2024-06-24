#pragma once
#include "windows/nodes/base_node.hpp"

class RenderpassImageNodeResource final : public gflow::parser::Resource
{
    EXPORT(std::string, ID);
    EXPORT_ENUM(usage, s_ImageUsageContext);

    void initContext(ExportData* metadata) override {}

public:
    DECLARE_RESOURCE(RenderpassImageNodeResource)

    static gflow::parser::EnumContext s_ImageUsageContext;
};

class RenderpassImageNode final : public GFlowNode
{
public:

    explicit RenderpassImageNode(ImGuiGraphWindow* parent);

    gflow::parser::Resource* getLinkedResource() override { return &m_resource; }
    void onResourceUpdated(const std::string& name, const std::string& path) override;

private:
    RenderpassImageNodeResource m_resource{""};
};

