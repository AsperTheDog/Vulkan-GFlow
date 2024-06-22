#pragma once
#include "imgui_resource_editor.hpp"
#include "ImNodeFlow.h"
#include "windows/imgui_editor_window.hpp"

class GFlowNode;

namespace gflow::parser
{
    class Resource;
}

class ImGuiRenderPassWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiRenderPassWindow(const std::string_view& name, bool defaultOpen = true);
    void resourceSelected(const std::string& resource);

    void draw() override;

    ImFlow::ImNodeFlow m_grid{"RenderPass"};

private:
    void rightClick(ImFlow::BaseNode* node);
    gflow::parser::Resource* m_selectedPass = nullptr;
    GFlowNode* m_sidePanelTarget = nullptr;
    ImGuiResourceEditorWindow m_sidePanel{"Side Panel", false};
};

