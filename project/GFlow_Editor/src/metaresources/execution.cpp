#include "execution.hpp"

gflow::parser::DataUsage BeginExecutionNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "renderpass")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
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

gflow::parser::DataUsage ImageNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "type")
        return gflow::parser::USED;
    if (variable == "imageID" && (*type).id == gflow::parser::EnumContexts::ExecutionImageType["Texture"])
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage ModelNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "path" || variable == "fields")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage ExternalArgumentNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "name")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage CameraNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "position" || variable == "rotation" || variable == "fov" || variable == "nearPlane" || variable == "farPlane")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage ObjectCameraNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "zoom" || variable == "target" || variable == "orbitPosition")
        return gflow::parser::USED;
    if (variable == "position" || variable == "rotation")
        return gflow::parser::NOT_USED;
    return CameraNodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage WatcherNodeResource::isUsed(const std::string& variable,
    const std::vector<Resource*>& parentPath)
{
    if (variable == "name")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}

gflow::parser::DataUsage PrimitiveNodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    if (variable == "value")
        return gflow::parser::USED;
    return NodeResource::isUsed(variable, parentPath);
}
