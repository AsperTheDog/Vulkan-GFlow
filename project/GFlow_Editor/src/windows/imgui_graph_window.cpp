#include "imgui_graph_window.hpp"
#include "nodes/base_node.hpp"

ImGuiGraphWindow::ImGuiGraphWindow(const std::string_view& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
{
    m_sidePanel.setInlinePadding(100.0f);
    m_grid.rightClickPopUpContent([this](ImFlow::BaseNode* node){this->rightClick(node);});
}

void ImGuiGraphWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    drawBody();
}

void ImGuiGraphWindow::drawBody()
{
    m_grid.update();
    ImGui::End();
    if (m_sidePanelTarget != nullptr)
        m_sidePanel.draw();
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
