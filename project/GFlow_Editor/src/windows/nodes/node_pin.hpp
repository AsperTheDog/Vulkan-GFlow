#pragma once
#include <string>

class GFlowNode;

class NodePin
{
public:
    explicit NodePin(GFlowNode* parent, std::string name) : m_parent(parent), m_name(std::move(name)) {}

    virtual void addStaticPins() {}
    virtual void drawDynamicPins() {}

    [[nodiscard]] std::string getName() const { return m_name; }

protected:
    GFlowNode* m_parent = nullptr;

    std::string m_name;
};