#pragma once
#include "ImNodeFlow.h"

class NodePin
{
public:
    explicit NodePin(ImFlow::BaseNode* parent, std::string name) : m_parent(parent), m_name(std::move(name)) {}

    virtual void addStaticPins() {}
    virtual void drawDynamicPins() {}

    std::string getName() const { return m_name; }

protected:
    ImFlow::BaseNode* m_parent = nullptr;

    std::string m_name;
};

