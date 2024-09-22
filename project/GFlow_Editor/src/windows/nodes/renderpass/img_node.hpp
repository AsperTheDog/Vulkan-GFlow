#pragma once
#include "metaresources/Renderpass.hpp"
#include "windows/nodes/base_node.hpp"

class ImageNode final : public GFlowNode
{
public:
    ImageNode(ImGuiGraphWindow* parent, gflow::parser::Resource* resource);

    gflow::parser::Resource* getLinkedResource() override { return m_resource; }
    void onResourceUpdated(const std::string& resource, const std::string& name, const std::string& path) override;

private:
    ImageNodeResource* m_resource = nullptr;
};