#pragma once
#include "graph.hpp"

class ImageNodeResource final : public NodeResource
{
    EXPORT(std::string, imageID);
    EXPORT_ENUM(usage, gflow::parser::EnumContexts::ImageUsageContext);

    void initContext(ExportData* metadata) override {}
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_RESOURCE_ANCESTOR(ImageNodeResource, NodeResource)
};

class SubpassNodeResource final : public NodeResource
{
    EXPORT(int, subpassID);

    void initContext(ExportData* metadata) override {}
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_RESOURCE_ANCESTOR(SubpassNodeResource, NodeResource)
};

class PipelineNodeResource final : public NodeResource
{
    void initContext(ExportData* metadata) override {}

public:
    DECLARE_RESOURCE_ANCESTOR(PipelineNodeResource, NodeResource)
};

class RenderpassResource final : public GraphResource
{

public:
    enum NodeType
    {
        IMAGE,
        SUBPASS,
        PIPELINE
    };

    NodeResource* addNode(NodeType type, gflow::parser::Vec2 position = {});
    void removeNode(int nodeID);

    DECLARE_RESOURCE_ANCESTOR_CUSTOM_CONST(RenderpassResource, GraphResource)
};

// **************
// Implementation
// **************

inline gflow::parser::DataUsage ImageNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "imageID" || variable == "usage")
    {
        return gflow::parser::USED;
    }
    return NodeResource::isUsed(variable, parentPath);
}

inline gflow::parser::DataUsage SubpassNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "subpassID")
    {
        return gflow::parser::USED;
    }
    return NodeResource::isUsed(variable, parentPath);
}

