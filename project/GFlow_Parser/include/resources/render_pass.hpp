#pragma once
#include "internal_list.hpp"
#include "../resource.hpp"
#include "pipeline.hpp"

namespace gflow::parser
{
    class RenderPassImage final : public Resource
    {
        EXPORT(std::string, imageID);
        EXPORT(int, referenceCount);
        EXPORT(bool, matchWindow);
        EXPORT(Vec2, size);
        EXPORT(bool, clear);

        void initContext(ExportData* metadata) override {}
        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    public:
        DECLARE_RESOURCE(RenderPassImage)

        template <typename T>
        friend class List;
    };

    class SubpassAttachment final : public Resource
    {
        EXPORT(std::string, imageID);
        EXPORT_ENUM(attachmentType, EnumContexts::attachmentType);

        void initContext(ExportData* metadata) override {}

    public:
        DECLARE_RESOURCE(SubpassAttachment)

        template <typename T>
        friend class List;
    };

    class RenderPassSubpass final : public Resource
    {
        EXPORT_RESOURCE_LIST(Pipeline, pipelines);
        EXPORT_RESOURCE_LIST(SubpassAttachment, attachments);

        void initContext(ExportData* metadata) override {}

        bool hasDepthAttachment();
    public:
        DECLARE_RESOURCE(RenderPassSubpass)

        template <typename T>
        friend class List;
    };

    class RenderPassCustomDependency final : public Resource
    {
        EXPORT(int, srcSubpass);
        EXPORT(int, destSubpass);

        void initContext(ExportData* metadata) override {}
    public:
        DECLARE_RESOURCE(RenderPassCustomDependency)

        template <typename T>
        friend class List;
    };

    class RenderPass final : public Resource
    {
        EXPORT_RESOURCE_LIST(RenderPassImage, attachments);
        EXPORT_RESOURCE_LIST(RenderPassCustomDependency, customDependencies);

        EXPORT_RESOURCE_LIST(RenderPassSubpass, subpasses);

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    public:
        DECLARE_RESOURCE(RenderPass)
    };

    inline DataUsage RenderPassImage::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (*matchWindow && variable == "size")
            return NOT_USED;
        if (variable == "referenceCount")
            return READ_ONLY;
        return USED;
    }

    inline bool RenderPassSubpass::hasDepthAttachment()
    {
        for (Pipeline* pipeline : (*pipelines).data())
        {
            PipelineDepthStencilState* depthState = pipeline->getValue<PipelineDepthStencilState*>("depthStencilState");
            if (depthState != nullptr && depthState->getValue<bool>("depthTestEnable"))
                return true;
        }
        return false;
    }

    inline DataUsage RenderPass::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "attachments" || variable == "customDependencies")
            return USED;
        return NOT_USED;
    }
}
