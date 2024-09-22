#pragma once
#include <functional>
#include <unordered_map>

#include "../resource.hpp"

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


