#pragma once
#include "imgui_graph_window.hpp"
#include "ImNodeFlow.h"
#include "nodes/renderpass_nodes.hpp"
#include "resources/renderpass.hpp"

class RenderpassResource;

namespace gflow::parser
{
    class RenderPass;
}

class GFlowNode;

class ImGuiRenderPassWindow final : public ImGuiGraphWindow
{
public:
    explicit ImGuiRenderPassWindow(const std::string& name, bool defaultOpen = true);

    void resourceSelected(const std::string& resource);
    void recreateParserData();

    void draw() override;

    void save() override;

private:
    void onNodeDestroyed(GFlowNode* node);
    void rightClick(ImFlow::BaseNode* node) override;

    void clearGrid() override;

    void saveRenderPass();
    void loadRenderPass(bool loadInit = true);

    void processSubpassConnections(SubpassNode* subpass, gflow::parser::RenderPassSubpass* subpassResource) const;
    void processPipelineConnections(SubpassPipelineNode* pipeline, PipelineNodeResource* pipelineResource) const;

    InitRenderpassNode* getInit();

    gflow::parser::RenderPass* m_selectedPass = nullptr;
    RenderpassResource* m_selectedPassMeta = nullptr;

    friend class NodeCreateHelper;
};
