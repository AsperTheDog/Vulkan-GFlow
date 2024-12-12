#pragma once
#include "imgui_graph_window.hpp"
#include "ImNodeFlow.h"
#include "metaresources/execution.hpp"
#include "nodes/execution_nodes.hpp"
#include "resources/project.hpp"

class InitExecutionNode;

class ImGuiExecutionWindow final : public ImGuiGraphWindow
{
public:
    explicit ImGuiExecutionWindow(const std::string& name, bool defaultOpen = true);
    void buildProject();

    void onProjectLoaded();

    void save() override;

private:
    void onNodeDestroyed(GFlowNode* node);
    void rightClick(ImFlow::BaseNode* node) override;

    void saveExecution();
    void loadExecution(bool loadInit = true);

    void processRenderpassConnections(BeginExecutionNode* renderpassNode, gflow::parser::ProjectRenderpass* renderpassResource) const;
    void processDrawCallConnections(DrawCallNode* drawNode, gflow::parser::ProjectRenderpassDrawCall* drawCallResource) const;

    InitExecutionNode* getInit();

    ExecutionResource* m_selectedExecMeta = nullptr;

    friend class NodeCreateHelper;
};

