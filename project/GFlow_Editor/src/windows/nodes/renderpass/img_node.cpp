#include "img_node.hpp"

#include "windows/imgui_graph_window.hpp"

ImageNode::ImageNode(ImGuiGraphWindow* parent, gflow::parser::Resource* resource)
    : GFlowNode("Image", parent), m_resource(dynamic_cast<ImageNodeResource*>(resource))
{
    m_resource->set("usage", "0");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,133,0,255), ImColor(233,241,244,255), 3.5f));
}

void ImageNode::onResourceUpdated(const std::string& resource, const std::string& name, const std::string& path)
{
    if (name != "imageID") return;
    const std::string newID = getLinkedResource()->get("imageID").first;
    setTitle("Image" + (newID.empty() ? "" : " (" + newID + ")"));
}
