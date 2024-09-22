#include "Renderpass.hpp"

RenderpassResource::RenderpassResource(const std::string& path) : GraphResource(path)
{
    nodes.setData(gflow::parser::ResourceManager::createResource("", Resource::create<gflow::parser::List<NodeResource*>>));
    connections.setData(gflow::parser::ResourceManager::createResource("", Resource::create<gflow::parser::List<gflow::parser::IntPair*>>));
}

NodeResource* RenderpassResource::addNode(const NodeType type, const gflow::parser::Vec2 position)
{
    NodeResource* node;
    switch (type)
    {
    case IMAGE:
        node = (*nodes).emplace_subclass_back<ImageNodeResource>();
        break;
    case SUBPASS:
        node = (*nodes).emplace_subclass_back<SubpassNodeResource>();
        break;
    case PIPELINE:
        node = (*nodes).emplace_subclass_back<PipelineNodeResource>();
        break;
    default:
        throw std::runtime_error("Newly implemented NodeType enum entry was not added to addNode...");
    }

    node->set("position", position.toString());
    return node;
}

void RenderpassResource::removeNode(const int nodeID)
{
    for (int i = 0; i < (*nodes).size(); i++)
    {
        if ((*nodes)[i]->getValue<int>("nodeID") == nodeID)
        {
            (*nodes).remove(i);
            break;
        }
    }
}

