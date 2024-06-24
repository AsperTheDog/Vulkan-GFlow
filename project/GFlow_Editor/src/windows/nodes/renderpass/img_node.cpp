#include "img_node.hpp"

gflow::parser::EnumContext RenderpassImageNodeResource::s_ImageUsageContext = {
    {
        "Read",
        "Write",
        "Read and Write"
    },
    {
        0,
        1,
        2
    }
};

RenderpassImageNode::RenderpassImageNode() : GFlowNode("Image")
{
    m_resource.set("usage", "0", {});
}
