#pragma once
#include "graph.hpp"
#include "resources/pipeline.hpp"
#include "windows/nodes/base_node.hpp"

class ImageNodeResource final : public NodeResource
{
    EXPORT(std::string, imageID);
    EXPORT_ENUM(usage, gflow::parser::EnumContexts::ImageUsageContext);

    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ImageNodeResource, NodeResource)
};

class PushConstantNodeResource final : public NodeResource
{
    EXPORT(std::string, structID);

    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PushConstantNodeResource, NodeResource)
};

class SubpassNodeResource final : public NodeResource
{
    EXPORT(int, subpassID);

    EXPORT_LIST(std::string, colorAttachments);
    EXPORT_LIST(std::string, inputAttachments);
    EXPORT(bool, depthAttachment);

    void initContext(ExportData* metadata) override;
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(SubpassNodeResource, NodeResource)

    std::vector<std::string> getColorAttachments() { return (*colorAttachments).data(); }
    std::vector<std::string> getInputAttachments() { return (*inputAttachments).data(); }
    bool hasDepthAttachment() { return *depthAttachment; }

    void addColorAttachment(const std::string& name) { (*colorAttachments).push_back(name); }
    void removeColorAttachment(const std::string& name) { (*colorAttachments).erase(name); }
    void addInputAttachment(const std::string& name) { (*inputAttachments).push_back(name); }
    void removeInputAttachment(const std::string& name) { (*inputAttachments).erase(name); }
    void setDepthAttachment(const bool enabled) { *depthAttachment = enabled; }

    void clearAttachments();
};

class PipelineNodeResource final : public NodeResource
{
    EXPORT_RESOURCE(gflow::parser::Pipeline, pipeline, false, true);

    EXPORT_LIST(std::string, pushConstants);
    
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    gflow::parser::Pipeline* getPipeline() { return *pipeline; }

    std::vector<std::string> getPushConstants() { return (*pushConstants).data(); }

    void addPushConstant(const std::string& name) { (*pushConstants).push_back(name); }
    void removePushConstant(const std::string& name) { (*pushConstants).erase(name); }

    void clearPushConstants() { (*pushConstants).clear(); }

    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PipelineNodeResource, NodeResource)
};

class RenderpassResource final : public GraphResource
{

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(RenderpassResource, GraphResource)
};