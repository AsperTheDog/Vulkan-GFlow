#pragma once

#include "ImNodeFlow.h"
#include "windows/nodes/node_pin.hpp"

class SubpassNode;

class InitPassOutputPin final : public NodePin
{
public:
	InitPassOutputPin(GFlowNode* parent, const std::string& name) : NodePin(parent, name) {}

	void addStaticPins() override;

	[[nodiscard]] static int behaviour() { return 0; }

private:
	std::shared_ptr<ImFlow::OutPin<int>> m_pin;
};

class SubpassOutputPin final : public NodePin
{
public:
	SubpassOutputPin(SubpassNode* parent, const std::string& name) : NodePin(reinterpret_cast<GFlowNode*>(parent), name) {}

	void addStaticPins() override;

	[[nodiscard]] int behaviour() const;

private:
	std::shared_ptr<ImFlow::OutPin<int>> m_pin;
};

class SubpassInputPin final : public NodePin
{
public:
	SubpassInputPin(SubpassNode* parent, const std::string& name) : NodePin(reinterpret_cast<GFlowNode*>(parent), name) {}

	void addStaticPins() override;

	static bool filter(const ImFlow::Pin* pin1, const ImFlow::Pin* pin2);

private:
	std::shared_ptr<ImFlow::InPin<int>> m_pin;
};

class PipelineOutputPin final : public NodePin
{
public:
	PipelineOutputPin(GFlowNode* parent, const std::string& name) : NodePin(parent, name) {}

	void addStaticPins() override;

	[[nodiscard]] int behaviour() const;

private:
	std::shared_ptr<ImFlow::OutPin<int>> m_pin;
};

class PipelineInputPin final : public NodePin
{
public:
	PipelineInputPin(GFlowNode* parent, const std::string& name) : NodePin(parent, name) {}

	void addStaticPins() override;

	static bool filter(const ImFlow::Pin* pin1, const ImFlow::Pin* pin2);

private:
	std::shared_ptr<ImFlow::InPin<int>> m_pin;
};