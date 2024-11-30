#include "renderpass.hpp"

#include "windows/nodes/base_node.hpp"

gflow::parser::DataUsage ImageNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "imageID" || variable == "usage")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage PushConstantNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "structID")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

void SubpassNodeResource::initContext(ExportData* metadata)
{
    if (colorAttachments.isNull())
        initializeExport(colorAttachments.getName());

    if (inputAttachments.isNull())
        initializeExport(inputAttachments.getName());
}

void SubpassNodeResource::clearAttachments()
{
    (*colorAttachments).clear();
    (*inputAttachments).clear();
    *depthAttachment = false;
}

gflow::parser::DataUsage SubpassNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "subpassID")
    {
        return gflow::parser::USED;
    }
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage PipelineNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "pipeline")
    {
        return gflow::parser::USED;
    }
    return NodeResource::isUsed(variable, parentPath);
}

