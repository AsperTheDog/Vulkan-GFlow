#pragma once
#include "graph.hpp"
#include "resources/pipeline.hpp"
#include "windows/nodes/base_node.hpp"

class InitNodeResource final : public NodeResource
{
    void initContext(ExportData* metadata) override {}

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(InitNodeResource, NodeResource)
};

class ImageNodeResource final : public NodeResource
{
    EXPORT(std::string, imageID);
    EXPORT_ENUM(usage, gflow::parser::EnumContexts::ImageUsageContext);

    void initContext(ExportData* metadata) override {}
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override { return gflow::parser::USED; }

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ImageNodeResource, NodeResource)
};

class PushConstantNodeResource final : public NodeResource
{
    EXPORT(std::string, structID);

    void initContext(ExportData* metadata) override {}
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override { return gflow::parser::USED; }

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
    
    void initContext(ExportData* metadata) override {}
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

    template <typename U>
    U* addNode(gflow::parser::Vec2 position = {});
    void removeNode(GFlowNode* node);

    void addConnection(size_t left_uid, size_t left_pin, size_t right_uid, size_t right_pin);
    void clearConnections() { (*connections).clear(); }

    DECLARE_PRIVATE_RESOURCE_ANCESTOR(RenderpassResource, GraphResource)
};

// **************
// Implementation
// **************

inline gflow::parser::DataUsage SubpassNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "subpassID")
    {
        return gflow::parser::USED;
    }
    return NodeResource::isUsed(variable, parentPath);
}

inline gflow::parser::DataUsage PipelineNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "pipeline")
    {
        return gflow::parser::USED;
    }
    return NodeResource::isUsed(variable, parentPath);
}

template <typename U>
U* RenderpassResource::addNode(const gflow::parser::Vec2 position)
{
    static_assert(std::is_base_of_v<NodeResource, U>, "T must be a subclass of NodeResource");

    U* node = (*nodes).emplace_subclass_back<U>();
    node->set("position", position.toString());
    return node;
}