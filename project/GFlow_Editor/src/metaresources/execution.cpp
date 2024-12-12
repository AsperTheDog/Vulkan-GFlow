#include "execution.hpp"

gflow::parser::DataUsage BeginExecutionNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
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
    if (*manualVertexCount && variable == "vertexCount")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage ImageNodeResource::isUsed(const std::string& variable,
                                                   const std::vector<Resource*>& parentPath)
{
    if (variable == "source")
        return gflow::parser::USED;

    switch ((*source).id)
    {
    case 2: // ImageSource::File
        if (variable == "path")
            return gflow::parser::USED;
        break;
    case 1: // ImageSource::Flat Color
        if (variable == "image")
            return gflow::parser::USED;
        break;
    }
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage ModelNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "path" || variable == "fields")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}
