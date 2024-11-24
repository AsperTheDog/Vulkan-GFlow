#pragma once
#include "graph.hpp"
#include "resources/renderpass.hpp"

class BeginExecutionNodeResource final : public NodeResource
{
    EXPORT_RESOURCE(gflow::parser::RenderPass, renderpass, false, true);

    EXPORT_LIST(std::string, attachments);

    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(BeginExecutionNodeResource, NodeResource)

    gflow::parser::RenderPass* getRenderpass() { return *renderpass; }

    std::vector<std::string> getAttachments() { return (*attachments).data(); }

    void addAttachment(const std::string& name) { (*attachments).push_back(name); }
    void removeAttachment(const std::string& name) { (*attachments).erase(name); }

    void clearAttachments();
};

class NextExecutionNodeResource final : public NodeResource
{

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(NextExecutionNodeResource, NodeResource)
};

class EndExecutionNodeResource final : public NodeResource
{

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(EndExecutionNodeResource, NodeResource)
};

class BindPushConstantNodeResource final : public NodeResource
{
    EXPORT(std::string, structID);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(BindPushConstantNodeResource, NodeResource)
};

class DrawCallNodeResource final : public NodeResource
{
    EXPORT_RESOURCE(gflow::parser::Pipeline, pipeline, false, true);

    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(DrawCallNodeResource, NodeResource)
};

class ExecutionResource final : public GraphResource
{

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ExecutionResource, GraphResource)
};