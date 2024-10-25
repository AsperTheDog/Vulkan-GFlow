#pragma once
#include "../resource_manager.hpp"

#include "image.hpp"
#include "pipeline.hpp"

namespace gflow::parser
{
    class SubpassAttachment final : public Resource
    {
        EXPORT(std::string, imageID);
        EXPORT_ENUM(attachmentType, EnumContexts::attachmentType);

        void initContext(ExportData* metadata) override {}

    public:
        DECLARE_PRIVATE_RESOURCE(SubpassAttachment)

        template <typename T>
        friend class List;
    };

    class SubpassPipeline final : public Resource
    {
        EXPORT_RESOURCE(Pipeline, pipeline);

        void initContext(ExportData* metadata) override {}

    public:
        Pipeline* getPipeline() { return *pipeline; }

        DECLARE_PRIVATE_RESOURCE(SubpassPipeline)

        template <typename T>
        friend class List;
    };

    class RenderPassSubpass final : public Resource
    {
        EXPORT_RESOURCE_LIST(SubpassPipeline, pipelines);
        EXPORT_RESOURCE_LIST(SubpassAttachment, attachments);

        void initContext(ExportData* metadata) override {}
    public:
        SubpassPipeline* addPipeline() { return *(*pipelines).emplace_back(); }
        SubpassAttachment* addAttachment() { return *(*attachments).emplace_back(); }

        bool hasDepthAttachment();

        DECLARE_PRIVATE_RESOURCE(RenderPassSubpass)

        template <typename T>
        friend class List;
    };

    class RenderPassCustomDependency final : public Resource
    {
        EXPORT(int, srcSubpass);
        EXPORT(int, destSubpass);

        void initContext(ExportData* metadata) override {}
    public:
        DECLARE_PRIVATE_RESOURCE(RenderPassCustomDependency)

        template <typename T>
        friend class List;
    };

    class RenderPass final : public Resource
    {
        EXPORT_RESOURCE_LIST(Image, attachments);
        EXPORT_RESOURCE_LIST(RenderPassCustomDependency, customDependencies);

        EXPORT_RESOURCE_LIST(RenderPassSubpass, subpasses);

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

    public:
        void clearSubpasses() { (*subpasses).clear(); }
        RenderPassSubpass* addSubpass() { return *(*subpasses).emplace_back(); }

        DECLARE_PUBLIC_RESOURCE(RenderPass)
    };

    inline bool RenderPassSubpass::hasDepthAttachment()
    {
        for (SubpassPipeline* pipeline : (*pipelines).data())
        {
            PipelineDepthStencilState* depthState = pipeline->getPipeline()->getValue<PipelineDepthStencilState*>("depthStencilState");
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
