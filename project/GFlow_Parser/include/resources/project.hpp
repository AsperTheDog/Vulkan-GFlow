#pragma once
#include <functional>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "resource.hpp"

namespace gflow::parser
{
    class Project final : public Resource
    {
    public:
        [[nodiscard]] std::string getName() const { return *name; }

    private:
        EXPORT(std::string, name);
        
    public:
        DECLARE_RESOURCE(Project)
    };
}


