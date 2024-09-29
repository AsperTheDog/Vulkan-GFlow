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
    void resourceVariableChanged(const gflow::parser::ResourceElemPath& element);
    void sidePanelVariableChanged(const gflow::parser::ResourceElemPath& element);

    void draw() override;

    void save() override;

private:
    void onNodeDestroyed(GFlowNode* node);
    void rightClick(ImFlow::BaseNode* node) override;

    void clearGrid() override;

    void onNodeCreated(ImFlow::BaseNode* node) override;
    void onNodeDeleted(ImFlow::BaseNode* node) override;
    void onConnection(ImFlow::Pin* pin1, ImFlow::Pin* pin2) override;

    void saveRenderPass();
    void loadRenderPass(bool loadInit = true);

    gflow::parser::RenderPass* m_selectedPass = nullptr;
    RenderpassResource* m_selectedPassMeta = nullptr;
};
