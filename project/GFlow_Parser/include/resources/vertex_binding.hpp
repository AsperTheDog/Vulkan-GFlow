#pragma once
#include "enum_contexts.hpp"
#include "list.hpp"
#include "resource.hpp"

namespace gflow::parser
{
    class VertexAttribute final : public Resource
    {
        EXPORT_ENUM(format, EnumContexts::format);
        EXPORT(int, offset);

        explicit VertexAttribute(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(VertexAttribute)

        template <typename T>
        friend class List;
    };

    class VertexBinding final : public Resource
    {
        EXPORT(int, stride);
        EXPORT_RESOURCE_LIST(VertexAttribute, attributes);

        explicit VertexBinding(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(VertexBinding)

        template <typename T>
        friend class List;
    };

    inline Resource* VertexAttribute::create(const std::string& path, const ExportData* metadata)
    {
        VertexAttribute* va = new VertexAttribute(path);
        if (!va->deserialize())
            va->serialize();
        return va;
    }

    inline Resource* VertexBinding::create(const std::string& path, const ExportData* metadata)
    {
        VertexBinding* vb = new VertexBinding(path);
        if (!vb->deserialize())
            vb->serialize();
        return vb;
    }
}
