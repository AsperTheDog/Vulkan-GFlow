#pragma once
#include "imgui_editor_window.hpp"
#include "imgui_resource_editor.hpp"
#include "ImNodeFlow.h"

class GFlowNode;

class ImGuiGraphWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiGraphWindow(const std::string_view& name, bool defaultOpen = true);
    void draw() override;

    [[nodiscard]] Signal<const std::string&, const std::string&, const std::string&>& getSidePanelUpdateSignal() { return m_sidePanel.getVariableChangedSignal(); }

protected:
    virtual void rightClick(ImFlow::BaseNode* node);
    void drawBody();

    ImFlow::ImNodeFlow m_grid{"RenderPass"};

    GFlowNode* m_sidePanelTarget = nullptr;
    ImGuiResourceEditorWindow m_sidePanel{"Side Panel", false};
};

