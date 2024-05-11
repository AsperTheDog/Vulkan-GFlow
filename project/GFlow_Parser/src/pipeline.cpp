#include "pipeline.hpp"

namespace gflow::parser
{
    Pipeline::Pipeline(const std::string_view path, Project* parent) : Resource(path, parent) {}

    std::string Pipeline::getType() const
    {
        return "Pipeline";
    }
}
