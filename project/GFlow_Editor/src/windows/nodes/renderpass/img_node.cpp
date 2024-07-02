#include "img_node.hpp"

gflow::parser::EnumContext RenderpassImageNodeResource::s_ImageUsageContext = {
    {
        "Read",
        "Write",
        "Read and Write"
    },
    {
        0,
        1,
        2
    }
};

RenderpassImageNode::RenderpassImageNode(ImGuiGraphWindow* parent) : GFlowNode("Image", parent)
{
    m_resource.set("usage", "0");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,133,0,255), ImColor(233,241,244,255), 3.5f));
}

void RenderpassImageNode::onResourceUpdated(const std::string& name, const std::string& path)
{
    if (name != "ID") return;
    const std::string newID = getLinkedResource()->get("ID").first;
    setTitle("Image" + (newID.empty() ? "" : " (" + newID + ")"));
}
