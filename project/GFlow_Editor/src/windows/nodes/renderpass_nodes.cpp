#include "renderpass_nodes.hpp"

#include "metaresources/Renderpass.hpp"

ImageNode::ImageNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("Image", parent), m_resource(dynamic_cast<ImageNodeResource*>(resource))
{
    m_resource->set("usage", "0");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,133,0,255), ImColor(233,241,244,255), 3.5f));
    
    m_out = addOUT<int>("-->", ImFlow::PinStyle::brown());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(IMAGE);
}

NodeResource* ImageNode::getLinkedResource()
{
    return m_resource;
}

void ImageNode::onResourceUpdated(const gflow::parser::ResourceElemPath& element)
{
    if (element.element != "imageID") return;
    const std::string newID = getLinkedResource()->get("imageID").first;
    setTitle("Image" + (newID.empty() ? "" : " (" + newID + ")"));
}

SubpassPipelineNode::SubpassPipelineNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("Pipeline", parent), m_resource(dynamic_cast<PipelineNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(109,0,181,255), ImColor(233,241,244,255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(PIPELINE, SUBPASS), ImFlow::PinStyle::white());
	m_in->setFilterID(PIPELINE);

    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(PIPELINE);
}

NodeResource* SubpassPipelineNode::getLinkedResource()
{
    return m_resource;
}

GFlowNode* SubpassPipelineNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

SubpassNode::SubpassNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("Subpass", parent), m_resource(dynamic_cast<SubpassNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,60,0,255), ImColor(233,241,244,255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(PIPELINE, INIT), ImFlow::PinStyle::white());
	m_in->setFilterID(SUBPASS);

    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(SUBPASS);
}

NodeResource* SubpassNode::getLinkedResource()
{
    return m_resource;
}

GFlowNode* SubpassNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

InitRenderpassNode::InitRenderpassNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("START", parent), m_resource(dynamic_cast<InitNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(71,142,173,255), ImColor(233,241,244,255), 3.5f));

    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->setFilterID(INIT);
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
}

NodeResource* InitRenderpassNode::getLinkedResource()
{
    return m_resource;
}

GFlowNode* InitRenderpassNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}
