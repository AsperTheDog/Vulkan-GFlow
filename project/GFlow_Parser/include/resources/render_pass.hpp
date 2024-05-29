#pragma once
#include "list.hpp"
#include "resource.hpp"

namespace gflow::parser
{
    class RenderPassCustomDependency final : public Resource
    {
        EXPORT(int, srcSubpass);
        EXPORT(int, destSubpass);
        
    public:
        DECLARE_RESOURCE(RenderPassCustomDependency)

        template <typename T>
        friend class List;
    };

    class RenderPassAttachment final : public Resource
    {
        EXPORT_ENUM(format, EnumContexts::format);
        EXPORT(bool, clear);
        
    public:
        DECLARE_RESOURCE(RenderPassAttachment)

        template <typename T>
        friend class List;
    };

    class RenderPass final : public Resource
    {
        EXPORT_RESOURCE_LIST(RenderPassAttachment, attachments);
        EXPORT_RESOURCE_LIST(RenderPassCustomDependency, customDependencies);

    public:
        DECLARE_RESOURCE(RenderPass)
    };
}
