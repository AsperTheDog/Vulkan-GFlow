#include "pipeline.hpp"

#include <vulkan/vulkan_core.h>

namespace gflow::parser
{
    Pipeline::Pipeline(const std::string& path) : Resource(path) {}

    std::string Pipeline::getType() const
    {
        return "Pipeline";
    }

    Resource* Pipeline::create(const std::string& path)
    {
        Pipeline* pipeline = new Pipeline(path);
        if (!pipeline->deserialize())
            pipeline->serialize();
        return pipeline;
    }

    EnumContext Pipeline::s_primitiveTopology{
        {
            "Point List",
            "Line List",
            "Line Strip",
            "Triangle List",
            "Triangle Strip",
            "Triangle Fan",
            "Line List With Adjacency",
            "Line Strip With Adjacency",
            "Triangle List With Adjacency",
            "Triangle Strip With Adjacency",
            "Patch List"
        },
        {
            VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
            VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
            VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
            VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
            VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
            VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
        }
    };
}
