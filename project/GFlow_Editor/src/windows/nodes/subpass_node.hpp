#pragma once
#include "windows/nodes/node_pin.hpp"

class SubpassFlowInputPin final : public NodePin
{
public:
    explicit SubpassFlowInputPin(ImFlow::BaseNode* parent, const std::string& name);

    void addStaticPins() override;
    static bool filter(ImFlow::Pin* src, ImFlow::Pin* dst);
};

class SubpassFlowOutputPin final : public NodePin
{
public:
    explicit SubpassFlowOutputPin(ImFlow::BaseNode* parent, const std::string& name) : NodePin(parent, name) {}

    void addStaticPins() override;
    static bool filter(ImFlow::Pin* src, ImFlow::Pin* dst);

    [[nodiscard]] uint32_t behavior() const;
};

class AttachmentInputPin final : public NodePin
{
public:
    explicit AttachmentInputPin(ImFlow::BaseNode* parent, const std::string& name) : NodePin(parent, name) {}

    void addStaticPins() override;
    static bool filter(ImFlow::Pin* src, ImFlow::Pin* dst);
};

// NODES

class InitSubpassNode final : public ImFlow::BaseNode
{
public:
    explicit InitSubpassNode();
    [[nodiscard]] uint32_t getIndex() const { return m_subpassIndex; }

    void destroy() override {}

private:
    SubpassFlowOutputPin m_subpassFlowOut{this, "EXIT"};

    uint32_t m_subpassIndex;
};

class SubpassNode final : public ImFlow::BaseNode
{
public:
    explicit SubpassNode(uint32_t subpassIndex);
    [[nodiscard]] uint32_t getIndex() const { return m_subpassIndex; }

    void draw() override;

private:
    SubpassFlowInputPin m_subpassFlowIn{this, "ENTER"};
    SubpassFlowOutputPin m_subpassFlowOut{this, "EXIT"};
    AttachmentInputPin m_depthAttachment{this, "depth attachment"};

    uint32_t m_subpassIndex;
};

