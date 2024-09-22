#pragma once
#include "ImNodeFlow.h"
#include "resource.hpp"
#include "utils/signal.hpp"

class ImGuiGraphWindow;

class GFlowNode : public ImFlow::BaseNode
{
public:
    explicit GFlowNode(const std::string& name, ImGuiGraphWindow* parent);

    virtual gflow::parser::Resource* getLinkedResource() = 0;
    void setInspectionStatus(bool status);

    void destroy() override;

    virtual void onResourceUpdated(const std::string& resource, const std::string& name, const std::string& path) {}
    [[nodiscard]] Signal<GFlowNode*>& getDestroyedSignal() { return m_destroyed; }

protected:
    Signal<bool> m_inspectionStatusChanged;

private:
    ImGuiGraphWindow* m_parent = nullptr;
    Signal<GFlowNode*> m_destroyed;
};
