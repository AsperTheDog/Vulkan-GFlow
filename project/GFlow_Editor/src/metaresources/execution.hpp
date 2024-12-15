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

    void clearAttachments() { (*attachments).clear(); }
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
    EXPORT(bool, manualVertexCount);
    EXPORT(int, vertexCount);
    EXPORT(bool, modelPin);

    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    gflow::parser::Pipeline* getPipeline() { return *pipeline; }
    void setModelPin(const bool enabled) { *modelPin = enabled; }
    bool hasModelPin() { return *modelPin; }

    DECLARE_PRIVATE_RESOURCE_ANCESTOR(DrawCallNodeResource, NodeResource)
};

class ImageNodeResource final : public NodeResource
{
    EXPORT_ENUM(type, gflow::parser::EnumContexts::ExecutionImageType);
    EXPORT(std::string, imageID);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ImageNodeResource, NodeResource)
};

class ModelNodeResource final : public NodeResource
{
    EXPORT(gflow::parser::FilePath, path);
    EXPORT_ENUM_LIST(fields, gflow::parser::EnumContexts::ModelFields);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ModelNodeResource, NodeResource)
};

class DataDecomposeNodeResource final : public NodeResource
{
    EXPORT_LIST(std::string, components);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override { return gflow::parser::NOT_USED; }

public:
    std::vector<std::string> getComponents() { return (*components).data(); }

    void addComponent(const std::string& name) { (*components).push_back(name); }
    void removeComponent(const std::string& name) { (*components).erase(name); }

    void clearComponents() { (*components).clear(); }

    DECLARE_PRIVATE_RESOURCE_ANCESTOR(DataDecomposeNodeResource, NodeResource)
};

class ExternalArgumentNodeResource final : public NodeResource
{
    EXPORT(std::string, name);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ExternalArgumentNodeResource, NodeResource)
};

class CameraNodeResource : public NodeResource
{
    EXPORT(gflow::parser::Vec3, position);
    EXPORT(gflow::parser::Vec3, rotation);
    EXPORT(float, fov);
    EXPORT(float, nearPlane);
    EXPORT(float, farPlane);

protected:
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(CameraNodeResource, NodeResource)
};

class ObjectCameraNodeResource final : public CameraNodeResource
{
    EXPORT(gflow::parser::Vec3, target);
    EXPORT(float, orbitPosition);
    EXPORT(float, zoom);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ObjectCameraNodeResource, CameraNodeResource)
};

class WatcherNodeResource final : public NodeResource
{
    EXPORT(std::string, name);
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(WatcherNodeResource, NodeResource)
};

// Primitives //

class PrimitiveNodeResource : public NodeResource
{
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveNodeResource, NodeResource)
};

class PrimitiveFloatNodeResource final : public PrimitiveNodeResource
{
    EXPORT(float, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveFloatNodeResource, PrimitiveNodeResource)

    static std::string getNodeName() { return "Float"; }
};

class PrimitiveIntNodeResource final : public PrimitiveNodeResource
{
    EXPORT(int, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveIntNodeResource, PrimitiveNodeResource)

    static std::string getNodeName() { return "Int"; }
};

class PrimitiveColorNodeResource final : public PrimitiveNodeResource
{
    EXPORT(gflow::parser::Color, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveColorNodeResource, PrimitiveNodeResource)

    static std::string getNodeName() { return "Color"; }
};

class PrimitiveVec2NodeResource final : public PrimitiveNodeResource
{
    EXPORT(gflow::parser::Vec2, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveVec2NodeResource, PrimitiveNodeResource)

    static std::string getNodeName() { return "Vec2"; }
};

class PrimitiveVec3NodeResource final : public PrimitiveNodeResource
{
    EXPORT(gflow::parser::Vec3, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveVec3NodeResource, PrimitiveNodeResource)

    static std::string getNodeName() { return "Vec3"; }
};

class PrimitiveVec4NodeResource final : public PrimitiveNodeResource
{
    EXPORT(gflow::parser::Vec4, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveVec4NodeResource, PrimitiveNodeResource)

    static std::string getNodeName() { return "Vec4"; }
};

class PrimitiveMat3NodeResource final : public PrimitiveNodeResource
{
    EXPORT(gflow::parser::Mat3, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveMat3NodeResource, PrimitiveNodeResource)
    static std::string getNodeName() { return "Mat3"; }
};

class PrimitiveMat4NodeResource final : public PrimitiveNodeResource
{
    EXPORT(gflow::parser::Mat4, value);
public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(PrimitiveMat4NodeResource, PrimitiveNodeResource)
    static std::string getNodeName() { return "Mat4"; }
};

////////////////

class ExecutionResource final : public GraphResource
{

public:
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(ExecutionResource, GraphResource)
};