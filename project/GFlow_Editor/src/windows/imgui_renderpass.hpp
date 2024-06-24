#pragma once
#include "imgui_graph_window.hpp"
#include "imgui_resource_editor.hpp"
#include "ImNodeFlow.h"

class GFlowNode;

namespace gflow::parser
{
    class Resource;
}

class ImGuiRenderPassWindow final : public ImGuiGraphWindow
{
public:
    explicit ImGuiRenderPassWindow(const std::string_view& name, bool defaultOpen = true);
    void resourceSelected(const std::string& resource);

private:
    void onNodeDestroyed(GFlowNode* node);
    void rightClick(ImFlow::BaseNode* node) override;

    gflow::parser::Resource* m_selectedPass = nullptr;
};
