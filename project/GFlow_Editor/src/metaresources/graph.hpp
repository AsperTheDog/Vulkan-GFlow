#pragma once
#include "resource.hpp"
#include "resources/list.hpp"
#include "resources/pair.hpp"

enum RenderPassPinType : uint8_t
{
    INIT,
    SUBPASS,
    PIPELINE,
    IMAGE
};

class NodeResource : public gflow::parser::Resource
{
private:
    EXPORT(size_t, nodeID);
    EXPORT(gflow::parser::Vec2, position);

protected:
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    
public:
    DECLARE_PRIVATE_RESOURCE(NodeResource)

    template <typename T>
    friend class gflow::parser::List;

    friend class GraphResource;

    [[nodiscard]] gflow::parser::Vec2 getPos() const { return *position; }
    [[nodiscard]] size_t getNodeID() const { return *nodeID; }

    void setPos(const gflow::parser::Vec2& pos) { *position = pos; }
    void setNodeID(const size_t id) { *nodeID = id; }

protected:
    NodeResource() = default;
};

class GraphResource : public gflow::parser::Resource
{
public:
    typedef gflow::parser::ResPair<gflow::parser::BigIntPair, gflow::parser::Pair<size_t, int>> Connection;
protected:

    EXPORT_RESOURCE_LIST(NodeResource, nodes);
    EXPORT_RESOURCE_LIST(Connection, connections);
    
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    gflow::parser::List<NodeResource*>& getNodes() { return *nodes; }
    gflow::parser::List<Connection*>& getConnections() { return *connections; }

    DECLARE_PRIVATE_RESOURCE(GraphResource)
};

// **************
// Implementation
// **************

inline gflow::parser::DataUsage NodeResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    return gflow::parser::NOT_USED;
}

inline gflow::parser::DataUsage GraphResource::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
{
    return gflow::parser::NOT_USED;
}

