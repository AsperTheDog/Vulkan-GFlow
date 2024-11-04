#pragma once
#include "../resource_manager.hpp"

#include "image.hpp"
#include "pipeline.hpp"

namespace gflow::parser
{
    class SubpassAttachment final
    {
    public:
        enum Type { DEPTH, COLOR, INPUT};

        SubpassAttachment(const std::string& imageID, const Type attachmentType) : imageID(imageID), attachmentType(attachmentType) {}

    private:
        std::string imageID;
        Type attachmentType;
    };

    class RenderPassSubpass final
    {
        using SubpassType = SubpassAttachment::Type;
    public:
        void addPipeline(Pipeline* pipeline) { return pipelines.push_back(pipeline); }
        SubpassAttachment& addAttachment(const std::string& imageID, const SubpassType attachmentType) { return attachments.emplace_back(imageID, attachmentType); }

        bool hasDepthAttachment() const;

    private:
        std::vector<Pipeline*> pipelines;
        std::vector<SubpassAttachment> attachments;
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

        std::vector<RenderPassSubpass> subpasses;

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

    public:
        void clearSubpasses() { subpasses.clear(); }
        RenderPassSubpass& addSubpass() { return subpasses.emplace_back(); }

        DECLARE_PUBLIC_RESOURCE(RenderPass)
    };

    inline bool RenderPassSubpass::hasDepthAttachment() const
    {
        for (Pipeline* pipeline : pipelines)
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
