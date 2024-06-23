#pragma once
#include "ImNodeFlow.h"
#include "resource.hpp"
#include "utils/signal.hpp"

class GFlowNode : public ImFlow::BaseNode
{
public:
    explicit GFlowNode(const std::string& name);

    virtual gflow::parser::Resource* getLinkedResource() = 0;
    void setInspectionStatus(bool status);

    void destroy() override;

    [[nodiscard]] Signal<GFlowNode*>& getDestroyedSignal() { return m_destroyed; }

private:
    Signal<GFlowNode*> m_destroyed;
};
