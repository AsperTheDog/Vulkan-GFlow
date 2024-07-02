#pragma once
#include "internal_list.hpp"
#include "resource.hpp"
#include "pipeline.hpp"

namespace gflow::parser
{
    class RenderPassImage final : public Resource
    {
        EXPORT(std::string, ID);
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

    class SubpassPipelineRef final : public Resource
    {
        EXPORT_RESOURCE(Pipeline, pipeline);

        void initContext(ExportData* metadata) override {}
    public:
        DECLARE_RESOURCE(SubpassPipelineRef)

        template <typename T>
        friend class List;
    };

    class RenderPassSubpass final : public Resource
    {
        EXPORT_RESOURCE_LIST(SubpassPipelineRef, pipelines);

        void initContext(ExportData* metadata) override {}
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
        EXPORT_RESOURCE_LIST(RenderPassSubpass, subpasses);
        EXPORT_INTERNAL_RESOURCE_LIST(RenderPassImage, attachments);
        EXPORT_RESOURCE_LIST(RenderPassCustomDependency, customDependencies);

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    public:
        DECLARE_RESOURCE(RenderPass)
    };

    inline DataUsage RenderPassImage::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (*matchWindow && variable == "size")
            return NOT_USED;
        if (variable == "referenceCount" || variable == "ID")
            return READ_ONLY;
        return USED;
    }

    inline DataUsage RenderPass::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "attachments" || variable == "customDependencies")
            return USED;
        return NOT_USED;
    }
}
