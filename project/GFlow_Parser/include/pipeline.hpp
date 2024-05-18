#pragma once
#include "resource.hpp"
#include "list.hpp"

#include "test.hpp"

namespace gflow::parser
{
    class Pipeline final : public Resource
    {
    public:
        [[nodiscard]] std::string getType() const override;

    private:
        EXPORT_LIST(Test*, testList);

        explicit Pipeline(const std::string& path);
        static Resource* create(const std::string& path);

        friend class ResourceManager;

    private:
        static EnumContext s_primitiveTopology;
    };
}
