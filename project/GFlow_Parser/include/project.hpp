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
        [[nodiscard]] std::string getType() const override;

    private:
        EXPORT(std::string, name);

        explicit Project(const std::string& path);
        static Resource* create(const std::string& path);

        friend class ResourceManager;
    };
}


