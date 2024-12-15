#pragma once
#include "ImNodeFlow.h"
#include "utils/signal.hpp"

namespace gflow::parser { struct ResourceElemPath; }

class ImGuiGraphWindow;
class NodeResource;

enum PinType : uint8_t
{
    INIT,
    SUBPASS,
    PIPELINE,
    BEGIN,
    NEXT,
    END,
    PUSH_CONSTANT,
    BIND_PUSH_CONSTANT,
    DRAW_CALL,
    IMAGE,
    MODEL,
    PRIMITIVE,
    EXTERNAL_ARGUMENT,
};

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

template <typename... Ints>
std::function<bool(const ImFlow::Pin*, const ImFlow::Pin*)> getLambdaFilter(Ints... filterIDs) {
    return [filterIDs...](const ImFlow::Pin* pin1, const ImFlow::Pin* pin2) -> bool { 
        int id = pin1->getFilterID();
        return ((id == filterIDs) || ...);  // Fold expression to match any filterID
    };
}
