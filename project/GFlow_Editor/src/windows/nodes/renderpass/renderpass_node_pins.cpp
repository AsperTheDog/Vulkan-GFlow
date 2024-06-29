#include "renderpass_node_pins.hpp"

#include "subpass_node.hpp"
#include "windows/nodes/base_node.hpp"

void InitPassOutputPin::addStaticPins()
{
	m_pin = m_parent->addOUT<int>(m_name, ImFlow::PinStyle::white());
	m_pin->setFilterID(0);
	m_pin->behaviour(&behaviour);
}

void SubpassOutputPin::addStaticPins()
{
	m_pin = m_parent->addOUT<int>(m_name, ImFlow::PinStyle::white());
	m_pin->setFilterID(1);
	m_pin->behaviour([this](){return this->behaviour(); });
}

int SubpassOutputPin::behaviour() const
{
	return dynamic_cast<SubpassNode*>(m_parent)->getLinkedResource()->getValue<int>("subpassID");
}

void SubpassInputPin::addStaticPins()
{
	m_pin = m_parent->addIN(m_name, -1, &filter, ImFlow::PinStyle::white());
	m_pin->setFilterID(1);
}

bool SubpassInputPin::filter(const ImFlow::Pin* pin1, const ImFlow::Pin* pin2)
{
	return pin1->getFilterID() == 0 || pin1->getFilterID() == 2;
}

void PipelineOutputPin::addStaticPins()
{
	m_pin = m_parent->addOUT<int>(m_name, ImFlow::PinStyle::white());
	m_pin->setFilterID(2);
	m_pin->behaviour([this](){return this->behaviour(); });
}

int PipelineOutputPin::behaviour() const
{
	return 1;
}

void PipelineInputPin::addStaticPins()
{
	m_pin = m_parent->addIN(m_name, -1, &filter, ImFlow::PinStyle::white());
	m_pin->setFilterID(2);
}

bool PipelineInputPin::filter(const ImFlow::Pin* pin1, const ImFlow::Pin* pin2)
{
	return pin1->getFilterID() == 1 || pin1->getFilterID() == 2;
}
