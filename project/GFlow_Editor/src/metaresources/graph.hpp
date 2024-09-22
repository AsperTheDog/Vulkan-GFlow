#pragma once
#include "resource.hpp"
#include "resources/list.hpp"
#include "resources/pair.hpp"

class NodeResource : public gflow::parser::Resource
{
    EXPORT(int, nodeID);
    EXPORT(gflow::parser::Vec2, position);
    EXPORT_RESOURCE_LIST(gflow::parser::IntPair, connections);

protected:
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    
public:
    DECLARE_RESOURCE(NodeResource)

    template <typename T>
    friend class gflow::parser::List;

    friend class GraphResource;

    void setPos(const gflow::parser::Vec2& pos) { *position = pos; }

protected:
    NodeResource() = default;
};

class GraphResource : public gflow::parser::Resource
{
protected:
    EXPORT_RESOURCE_LIST(NodeResource, nodes);
    EXPORT_RESOURCE_LIST(gflow::parser::IntPair, connections);
    
    gflow::parser::DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

public:
    DECLARE_RESOURCE(GraphResource)
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

