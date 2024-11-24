#include "renderpass_nodes.hpp"

#include "metaresources/renderpass.hpp"

ImageNode::ImageNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("Image", parent), m_resource(dynamic_cast<ImageNodeResource*>(resource))
{
    m_resource->set("usage", "0");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(99,156,0,255), ImColor(233,241,244,255), 3.5f));
    
    m_out = addOUT<int>("-->", ImFlow::PinStyle::green());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(IMAGE);

    const std::string currentID = getLinkedResource()->getValue<std::string>("imageID");
    setTitle("Image" + (currentID.empty() ? "" : " (" + currentID + ")"));
}

void ImageNode::onResourceUpdated(const gflow::parser::ResourceElemPath& element)
{
    if (element.element != "imageID") return;
    const std::string newID = getLinkedResource()->getValue<std::string>("imageID");
    setTitle("Image" + (newID.empty() ? "" : " (" + newID + ")"));
}

PushConstantNode::PushConstantNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("Image", parent), m_resource(dynamic_cast<PushConstantNodeResource*>(resource))
{
    m_resource->set("usage", "0");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(0,135,166,255), ImColor(233,241,244,255), 3.5f));
    
    m_out = addOUT<int>("-->", ImFlow::PinStyle::blue());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(PUSH_CONSTANT);

    const std::string currentID = getLinkedResource()->getValue<std::string>("structID");
    setTitle("Push" + (currentID.empty() ? "" : " (" + currentID + ")"));
}

void PushConstantNode::onResourceUpdated(const gflow::parser::ResourceElemPath& element)
{
    if (element.element != "structID") return;
    const std::string newID = getLinkedResource()->getValue<std::string>("structID");
    setTitle("Push" + (newID.empty() ? "" : " (" + newID + ")"));
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

    for (const std::string& attachment : m_resource->getPushConstants())
        addPushConstantPin(attachment, false);
}

GFlowNode* SubpassPipelineNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

void SubpassPipelineNode::addPushConstantPin(const std::string& name, const bool addToResource)
{
    static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(90,117,191,255), 4, 4.f, 4.67f, 4.2f, 1.3f));

    const std::shared_ptr<ImFlow::InPin<int>> newAttachment = addIN(name, 0, getLambdaFilter(PUSH_CONSTANT), pinColor);
    newAttachment->setFilterID(PUSH_CONSTANT);
    m_PushConstantPins.push_back(newAttachment);
    if (addToResource)
        m_resource->addPushConstant(name);
}

void SubpassPipelineNode::removePushConstantPin(const std::string& name)
{
    dropIN(name);
    m_resource->removePushConstant(name);
}

std::unordered_set<std::string> SubpassPipelineNode::getPushConstants() const
{
    std::unordered_set<std::string> pins;
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_PushConstantPins)
        pins.insert(pin->getName());
    return pins;
}

std::vector<std::string> SubpassPipelineNode::getColorAttachmentPins()
{
    std::vector<std::string> pins;
    for (const std::shared_ptr<ImFlow::Pin>& pin : getIns())
        if (pin->getFilterID() == IMAGE)
            pins.push_back(pin->getName());
    return pins;
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

    for (const std::string& attachment : m_resource->getColorAttachments())
        addColorAttachmentPin(attachment, false);

    for (const std::string& attachment : m_resource->getInputAttachments())
        addInputAttachmentPin(attachment, false);

    if (m_resource->hasDepthAttachment())
        setDepthAttachment(true, true);
}

GFlowNode* SubpassNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

void SubpassNode::addColorAttachmentPin(const std::string& name, const bool addToResource)
{
    static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(145,255,150,255), 4, 4.f, 4.67f, 4.2f, 1.3f));

    const std::shared_ptr<ImFlow::InPin<int>> newAttachment = addIN(name, 0, getLambdaFilter(IMAGE), pinColor);
    newAttachment->setFilterID(IMAGE);
    m_ColorAttachmentPins.push_back(newAttachment);
    if (addToResource)
        m_resource->addColorAttachment(name);
}

void SubpassNode::removeColorAttachmentPin(const std::string& name)
{
    dropIN(name);
    m_resource->removeColorAttachment(name);
}

void SubpassNode::addInputAttachmentPin(const std::string& name, const bool addToResource)
{
    static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(95,184,98,255), 4, 4.f, 4.67f, 4.2f, 1.3f));

    const std::shared_ptr<ImFlow::InPin<int>> newAttachment = addIN(name, 0, getLambdaFilter(IMAGE), pinColor);
    newAttachment->setFilterID(IMAGE);
    m_InputAttachmentPins.push_back(newAttachment);
    if (addToResource)
        m_resource->addInputAttachment(name);
}

void SubpassNode::removeInputAttachmentPin(const std::string& name)
{
    dropIN(name);
    m_resource->removeInputAttachment(name);
}

void SubpassNode::removeAllReflectionPins()
{
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_ColorAttachmentPins)
        dropIN(pin->getName());
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_InputAttachmentPins)
        dropIN(pin->getName());
    setDepthAttachment(false, false);
    m_resource->clearAttachments();
}

std::unordered_set<std::string> SubpassNode::getColorAttachments() const
{
    std::unordered_set<std::string> pins;
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_ColorAttachmentPins)
        pins.insert(pin->getName());
    return pins;
}

std::unordered_set<std::string> SubpassNode::getInputAttachments() const
{
    std::unordered_set<std::string> pins;
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_InputAttachmentPins)
        pins.insert(pin->getName());
    return pins;
}

void SubpassNode::setDepthAttachment(const bool enabled, const bool force)
{
    static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(49,130,52,255), 4, 4.f, 4.67f, 4.2f, 1.3f));

    if (m_resource->hasDepthAttachment() == enabled && !force) return;

    if (enabled)
    {
        m_DepthAttachmentPin = addIN("depth", 0, getLambdaFilter(IMAGE), pinColor);
        m_DepthAttachmentPin->setFilterID(IMAGE);
    }
    else
    {
        dropIN("depth");
    }
    m_resource->setDepthAttachment(enabled);
}

InitRenderpassNode::InitRenderpassNode(ImGuiGraphWindow* parent, NodeResource* resource)
    : GFlowNode("START", parent), m_resource(dynamic_cast<InitNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(71,142,173,255), ImColor(233,241,244,255), 3.5f));

    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->setFilterID(INIT);
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
}

GFlowNode* InitRenderpassNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}
