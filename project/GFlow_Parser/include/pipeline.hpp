#pragma once
#include "resource.hpp"
#include "list.hpp"

#include "test.hpp"

namespace gflow::parser
{
    class Pipeline final : public Resource
    {
    private:
        EXPORT_RESOURCE_LIST(Test, testList);
        EXPORT_ENUM(testEnum, s_primitiveTopology);
        EXPORT_ENUM_LIST(testEnumList, s_primitiveTopology);
        EXPORT_RESOURCE(Pipeline, parent);

        explicit Pipeline(const std::string& path);

        DECLARE_RESOURCE(Pipeline)

    private:
        static EnumContext s_primitiveTopology;
    };
}
