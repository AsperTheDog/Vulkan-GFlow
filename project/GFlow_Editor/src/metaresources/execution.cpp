#include "execution.hpp"

gflow::parser::DataUsage BeginExecutionNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "renderpass")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

void BeginExecutionNodeResource::clearAttachments()
{
    (*attachments).clear();
}

gflow::parser::DataUsage BindPushConstantNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "structID")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage DrawCallNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "pipeline")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}
