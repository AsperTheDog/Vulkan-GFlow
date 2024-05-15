#pragma once
#include "resource.hpp"
#include <test.hpp>

namespace gflow::parser
{
    class Pipeline final : public Resource
    {
    public:
        [[nodiscard]] std::string getType() const override;

    private:
        EXPORT(std::string, name);
        EXPORT(Test*, test);

        Pipeline(const std::string& path, Project* parent);
        static Resource* create(const std::string& path, Project* parent);

        friend class Project;

    private:
        static EnumContext s_primitiveTopology;
    };
}


