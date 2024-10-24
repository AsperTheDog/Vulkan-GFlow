#pragma once
#include "../resource_manager.hpp"

#include <functional>

namespace gflow::parser
{
    class Project final : public Resource
    {
    public:
        [[nodiscard]] std::string getName() const { return *name; }

    private:
        EXPORT(std::string, name);
        
    public:
        DECLARE_PRIVATE_RESOURCE(Project)
    };
}


