#pragma once
#include "ImNodeFlow.h"
#include "utils/signal.hpp"

namespace gflow::parser { struct ResourceElemPath; }

class ImGuiGraphWindow;
class NodeResource;

class GFlowNode : public ImFlow::BaseNode
{
public:
    explicit GFlowNode(const std::string& name, ImGuiGraphWindow* parent);

    virtual NodeResource* getLinkedResource() = 0;
    void setInspectionStatus(bool status);

    void destroy() override;

    virtual void onResourceUpdated(const gflow::parser::ResourceElemPath&) {}
    [[nodiscard]] Signal<GFlowNode*>& getDestroyedSignal() { return m_destroyed; }

protected:
    Signal<bool> m_inspectionStatusChanged;

private:
    ImGuiGraphWindow* m_parent = nullptr;
    Signal<GFlowNode*> m_destroyed;
};
