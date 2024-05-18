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
        EXPORT_ENUM(topology, s_primitiveTopology);
        EXPORT(Test*, test);
        EXPORT(bool, enabled);
        EXPORT(int, count);

        explicit Pipeline(const std::string& path);
        static Resource* create(const std::string& path);

        friend class ResourceManager;

    private:
        static EnumContext s_primitiveTopology;
    };
}


