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
    m_resource.set("usage", "0", {});
}

void RenderpassImageNode::onResourceUpdated(const std::string& name, const std::string& path)
{
    if (name != "ID") return;
    const std::string newID = getLinkedResource()->get("ID").first;
    setTitle("Image" + (newID.empty() ? "" : " (" + newID + ")"));
}
