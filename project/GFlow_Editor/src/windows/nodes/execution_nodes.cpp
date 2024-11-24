#include "execution_nodes.hpp"

InitExecutionNode::InitExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource)
 : GFlowNode("Init", parent), m_resource(dynamic_cast<InitNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(99,156,0,255), ImColor(233, 241, 244, 255), 3.5f));
    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(INIT);
}

GFlowNode* InitExecutionNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

BeginExecutionNode::BeginExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource)
 : GFlowNode("Begin", parent), m_resource(dynamic_cast<BeginExecutionNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(255,113,41,255), ImColor(233, 241, 244, 255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(INIT, END), ImFlow::PinStyle::white());
    m_in->setFilterID(BEGIN);
    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(BEGIN);

    for (const std::string& attachment : m_resource->getAttachments())
        addAttachmentPin(attachment, false);
}

GFlowNode* BeginExecutionNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

void BeginExecutionNode::addAttachmentPin(const std::string& name, const bool addToResource)
{
    static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(145,255,150,255), 4, 4.f, 4.67f, 4.2f, 1.3f));

    const std::shared_ptr<ImFlow::InPin<int>> newAttachment = addIN(name, 0, getLambdaFilter(IMAGE), pinColor);
    newAttachment->setFilterID(IMAGE);
    m_AttachmentPins.push_back(newAttachment);
    if (addToResource)
        m_resource->addAttachment(name);
}

void BeginExecutionNode::removeAttachmentPin(const std::string& name)
{
    dropIN(name);
    m_resource->removeAttachment(name);
}

void BeginExecutionNode::removeAllReflectionPins()
{
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_AttachmentPins)
        dropIN(pin->getName());
    m_resource->clearAttachments();
}

std::unordered_set<std::string> BeginExecutionNode::getAttachments() const
{
    std::unordered_set<std::string> pins;
    for (const std::shared_ptr<ImFlow::InPin<int>>& pin : m_AttachmentPins)
        pins.insert(pin->getName());
    return pins;
}

NextExecutionNode::NextExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource)
 : GFlowNode("Next", parent), m_resource(dynamic_cast<NextExecutionNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(181,60,0,255), ImColor(233, 241, 244, 255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(NEXT, BEGIN, BIND_PUSH_CONSTANT, DRAW_CALL), ImFlow::PinStyle::white());
    m_in->setFilterID(NEXT);
    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(NEXT);
}

GFlowNode* NextExecutionNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

EndExecutionNode::EndExecutionNode(ImGuiGraphWindow* parent, NodeResource* resource)
 : GFlowNode("End", parent), m_resource(dynamic_cast<EndExecutionNodeResource*>(resource))
{
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(107, 36, 0, 255), ImColor(233, 241, 244, 255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(NEXT, BEGIN, BIND_PUSH_CONSTANT, DRAW_CALL), ImFlow::PinStyle::white());
    m_in->setFilterID(END);
    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(END);
}

GFlowNode* EndExecutionNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

BindPushConstantNode::BindPushConstantNode(ImGuiGraphWindow* parent, NodeResource* resource)
 : GFlowNode("Bind Data", parent)
{
    m_resource = dynamic_cast<BindPushConstantNodeResource*>(resource);
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(0,135,166,255), ImColor(233, 241, 244, 255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(NEXT, BEGIN, DRAW_CALL), ImFlow::PinStyle::white());
    m_in->setFilterID(BIND_PUSH_CONSTANT);
    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(BIND_PUSH_CONSTANT);

    static std::shared_ptr<ImFlow::PinStyle> pinColor = std::make_shared<ImFlow::PinStyle>(ImFlow::PinStyle(IM_COL32(0,135,166,255), 4, 4.f, 4.67f, 4.2f, 1.3f));

    m_pushConstantData = addIN("Data", 0, getLambdaFilter(PUSH_CONSTANT), pinColor);
    m_pushConstantData->setFilterID(PUSH_CONSTANT);

    const std::string currentID = getLinkedResource()->getValue<std::string>("structID");
    setTitle("Bind Data" + (currentID.empty() ? "" : " (" + currentID + ")"));
}

GFlowNode* BindPushConstantNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}

void BindPushConstantNode::onResourceUpdated(const gflow::parser::ResourceElemPath& element)
{
    if (element.element != "structID") return;
    const std::string newID = getLinkedResource()->getValue<std::string>("structID");
    setTitle("Bind Data" + (newID.empty() ? "" : " (" + newID + ")"));
}

DrawCallNode::DrawCallNode(ImGuiGraphWindow* parent, NodeResource* resource)
 : GFlowNode("Draw Call", parent)
{
    m_resource = dynamic_cast<DrawCallNodeResource*>(resource);
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(109,0,181,255), ImColor(233, 241, 244, 255), 3.5f));
    m_in = addIN("-->", 0, getLambdaFilter(NEXT, BEGIN, BIND_PUSH_CONSTANT), ImFlow::PinStyle::white());
    m_in->setFilterID(DRAW_CALL);
    m_out = addOUT<int>("-->", ImFlow::PinStyle::white());
    m_out->behaviour([this]() -> int { return 0; }); //Not used, but needed to prevent segfault
    m_out->setFilterID(DRAW_CALL);
}

GFlowNode* DrawCallNode::getNext() const
{
    const std::weak_ptr<ImFlow::Link> link = m_out->getLink();
    if (link.expired()) return nullptr;
    return dynamic_cast<GFlowNode*>(link.lock()->right()->getParent());
}
