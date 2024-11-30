#pragma once
#include "imgui_editor_window.hpp"
#include "imgui_resource_editor.hpp"
#include "ImNodeFlow.h"

class GFlowNode;

class ImGuiGraphWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiGraphWindow(const std::string& name, bool defaultOpen = true);
    void draw() override;

    [[nodiscard]] Signal<const gflow::parser::ResourceElemPath&>& getSidePanelUpdateSignal() { return m_sidePanel.getVariableChangedSignal(); }

protected:
    virtual void rightClick(ImFlow::BaseNode* node);
    void drawBody();

    virtual void clearGrid();

    virtual void onConnection(ImFlow::Pin* pin1, ImFlow::Pin* pin2) {}
    virtual void onNodeCreated(ImFlow::BaseNode* node) {}
    virtual void onNodeDeleted(ImFlow::BaseNode* node) {}

    ImFlow::ImNodeFlow m_grid{"Grid"};

    GFlowNode* m_sidePanelTarget = nullptr;
    ImGuiResourceEditorWindow m_sidePanel;
    Signal<> m_refreshRequestedSignal;
};

