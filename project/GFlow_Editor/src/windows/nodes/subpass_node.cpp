#include "subpass_node.hpp"


SubpassFlowInputPin::SubpassFlowInputPin(ImFlow::BaseNode* parent, const std::string& name) : NodePin(parent, name)
{

}

void SubpassFlowInputPin::addStaticPins()
{
    m_parent->addIN<uint32_t>(m_name, 0, SubpassFlowInputPin::filter, ImFlow::PinStyle::white());
}

bool SubpassFlowInputPin::filter(ImFlow::Pin* src, ImFlow::Pin* dst)
{
    return true;
}

void SubpassFlowOutputPin::addStaticPins()
{
    m_parent->addOUT<uint32_t>(m_name, ImFlow::PinStyle::white())->behaviour([this](){ return this->behavior(); });
}

bool SubpassFlowOutputPin::filter(ImFlow::Pin* src, ImFlow::Pin* dst)
{
    return true;
}

uint32_t SubpassFlowOutputPin::behavior() const
{
    if (const SubpassNode* node = dynamic_cast<SubpassNode*>(m_parent); node != nullptr)
        return node->getIndex() + 1;
    if (const InitSubpassNode* node = dynamic_cast<InitSubpassNode*>(m_parent); node != nullptr)
        return 1;
    throw std::runtime_error("Invalid node type, this should never happen");
}

void AttachmentInputPin::addStaticPins()
{
    m_parent->addIN(m_name, 0, AttachmentInputPin::filter, ImFlow::PinStyle::blue());
}

bool AttachmentInputPin::filter(ImFlow::Pin* src, ImFlow::Pin* dst)
{
    return true;
}

InitSubpassNode::InitSubpassNode()
{
    setTitle("Start");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(71,142,173,255), ImColor(233,241,244,255), 3.5f));

    m_subpassFlowOut.addStaticPins();
}

SubpassNode::SubpassNode(const uint32_t subpassIndex) : m_subpassIndex(subpassIndex)
{
    setTitle("Subpass");
    setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(180,70,70,255), ImColor(233,241,244,255), 3.5f));

    m_subpassFlowIn.addStaticPins();
    m_subpassFlowOut.addStaticPins();
    m_depthAttachment.addStaticPins();
}

void SubpassNode::draw()
{
    const uint32_t index = getInVal<uint32_t, std::string>(m_subpassFlowIn.getName());
    if (index != m_subpassIndex)
    {
        m_subpassIndex = index;
        if (m_subpassIndex == 0)
        {
            setTitle("Subpass");
            setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(180,70,70,255), ImColor(233,241,244,255), 3.5f));
        }
        else
        {
            setTitle("Subpass " + std::to_string(index));
            setStyle(std::make_shared<ImFlow::NodeStyle>(IM_COL32(40,150,45,255), ImColor(233,241,244,255), 3.5f));
        }
    }
    
}
