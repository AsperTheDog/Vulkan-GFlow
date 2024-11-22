#pragma once
#include "resource.hpp"
#include "resources/list.hpp"
#include "resources/pair.hpp"

class CustomPin final : public gflow::parser::Resource
{
private:
    EXPORT(std::string, name);
    EXPORT(int, type);
    EXPORT(size_t, pinID);
    EXPORT(gflow::parser::UColor, color);
    EXPORT_LIST(int, filters);

public:
    DECLARE_PRIVATE_RESOURCE(CustomPin)
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

class Connection final : public gflow::parser::Resource
{
private:
    EXPORT(size_t, leftUID);
    EXPORT(size_t, leftPin);
    EXPORT(size_t, rightUID);
    EXPORT(size_t, rightPin);
    
public:
    [[nodiscard]] size_t getLeftUID() const { return *leftUID; }
    [[nodiscard]] size_t getLeftPin() const { return *leftPin; }
    [[nodiscard]] size_t getRightUID() const { return *rightUID; }
    [[nodiscard]] size_t getRightPin() const { return *rightPin; }
    void setValues(const size_t leftUID, const size_t leftPin, const size_t rightUID, const size_t rightPin)
    {
        *this->leftUID = leftUID;
        *this->leftPin = leftPin;
        *this->rightUID = rightUID;
        *this->rightPin = rightPin;
    }

    DECLARE_PRIVATE_RESOURCE(Connection)
};

class GraphResource : public gflow::parser::Resource
{
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

