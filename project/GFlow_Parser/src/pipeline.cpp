#include "pipeline.hpp"

namespace gflow::parser
{
    Pipeline::Pipeline(const std::string_view path, Project* parent) : Resource(path, parent) {}

    std::string Pipeline::getType() const
    {
        return "Pipeline";
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
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
        }
    };
}
