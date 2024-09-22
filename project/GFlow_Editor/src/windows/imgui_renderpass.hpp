#pragma once
#include "imgui_graph_window.hpp"
#include "imgui_resource_editor.hpp"
#include "ImNodeFlow.h"

class RenderpassResource;

namespace gflow::parser
{
    class RenderPass;
}

class GFlowNode;

class ImGuiRenderPassWindow final : public ImGuiGraphWindow
{
public:
    explicit ImGuiRenderPassWindow(const std::string_view& name, bool defaultOpen = true);

    void resourceSelected(const std::string& resource);
    void resourceVariableChanged(const std::string& resourcePath, const std::string& field, const std::string& subpath);

    void draw() override;

private:
    void onNodeDestroyed(GFlowNode* node);
    void rightClick(ImFlow::BaseNode* node) override;

    void saveRenderPass();
    void loadRenderPass();

    gflow::parser::RenderPass* m_selectedPass = nullptr;
    RenderpassResource* m_selectedPassMeta = nullptr;
};
