#include "imgui_graph_window.hpp"
#include "nodes/base_node.hpp"
#include "../metaresources/graph.hpp"

ImGuiGraphWindow::ImGuiGraphWindow(const std::string_view& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
{
    m_sidePanel.setInlinePadding(100.0f);
    clearGrid();
}

void ImGuiGraphWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    drawBody();
}

void ImGuiGraphWindow::drawBody()
{
    if (ImGui::Button("Recalculate"))
        m_refreshRequestedSignal.emit();
    m_grid.update();
    ImGui::End();
    if (m_sidePanelTarget != nullptr)
    {
        m_sidePanel.open = true;
        m_sidePanel.draw();
        if (!m_sidePanel.open)
        {
            m_sidePanelTarget->setInspectionStatus(false);
            m_sidePanelTarget = nullptr;
            m_sidePanel.resourceSelected(nullptr);
        }
    }

}

void ImGuiGraphWindow::clearGrid()
{
    m_grid = ImFlow::ImNodeFlow("Grid");
    m_grid.getNodeCreatedSignal().connect(this, &ImGuiGraphWindow::onNodeCreated);
    m_grid.getNodeDeletedSignal().connect(this, &ImGuiGraphWindow::onNodeDeleted);
    m_grid.getConnectionSignal().connect(this, &ImGuiGraphWindow::onConnection);
    m_grid.rightClickPopUpContent([this](ImFlow::BaseNode* node){this->rightClick(node);});
}

void ImGuiGraphWindow::rightClick(ImFlow::BaseNode* node)
{
    if (GFlowNode* gfNode = dynamic_cast<GFlowNode*>(node); gfNode != nullptr)
    {
        if (ImGui::MenuItem("Inspect"))
        {
            if (m_sidePanelTarget != nullptr)
                m_sidePanelTarget->setInspectionStatus(false);
            m_sidePanelTarget = gfNode;
            m_sidePanel.resourceSelected(m_sidePanelTarget->getLinkedResource());
            m_sidePanelTarget->setInspectionStatus(true);
        }
    }
}
