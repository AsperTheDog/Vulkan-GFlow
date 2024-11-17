#include "base_node.hpp"

#include "metaresources/graph.hpp"
#include "windows/imgui_graph_window.hpp"

GFlowNode::GFlowNode(const std::string& name, ImGuiGraphWindow* parent) : m_parent(parent)
{
    setTitle(name);
}

void GFlowNode::setInspectionStatus(const bool status)
{
    const std::shared_ptr<ImFlow::NodeStyle> currentStyle = getStyle();
    if (status)
    {
        currentStyle->border_color = IM_COL32(255, 85, 0, 255);
        currentStyle->border_selected_color = IM_COL32(255, 141, 84, 255);
        m_parent->getSidePanelUpdateSignal().connect(this, &GFlowNode::onResourceUpdated, "CurrentNode");
    }
    else
    {
        currentStyle->border_color = IM_COL32(30, 38, 41, 140);
        currentStyle->border_selected_color = IM_COL32(170, 190, 205, 230);
    }
    m_inspectionStatusChanged.emit(status);
}

void GFlowNode::destroy()
{
    BaseNode::destroy();
    m_destroyed.emit(this);
}
