#pragma once
#include "resource.hpp"

namespace gflow::parser
{
    class Pipeline final : public Resource
    {
    public:
        Pipeline(std::string_view path, Project* parent);

        [[nodiscard]] std::string getType() const override;

    private:
        EXPORT(std::string, name);
        EXPORT(std::vector<int>, indices1);
        EXPORT(std::vector<std::string>, indices2);
        EXPORT(std::vector<bool>, indices3);
        EXPORT(std::vector<float>, indices4);

        static Resource* create(const std::string_view path, Project* parent) { return new Pipeline(path, parent); }

        friend class Project;

    private:
        static EnumContext s_primitiveTopology;
    };
}


