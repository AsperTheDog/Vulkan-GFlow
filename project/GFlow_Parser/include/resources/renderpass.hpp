#pragma once
#include "../resource_manager.hpp"

#include "image.hpp"
#include "pipeline.hpp"

namespace gflow::parser
{
    class SubpassAttachment final : public Resource
    {
    public:
        enum Type : uint8_t { DEPTH, COLOR, INPUT};

    private:
        EXPORT(std::string, imageID);
        EXPORT(int, attachmentType);

    public:
        [[nodiscard]] const std::string& getImageID() const { return *imageID; }
        [[nodiscard]] Type getAttachmentType() const { return static_cast<Type>(*attachmentType); }

        void setImageID(const std::string& imageID) { this->imageID.setData(imageID); }
        void setAttachmentType(const Type attachmentType) { this->attachmentType.setData(attachmentType); }

        DECLARE_PRIVATE_RESOURCE(SubpassAttachment);

        template <typename T>
        friend class List;
    };

    class RenderPassSubpass final : public Resource
    {
        using SubpassType = SubpassAttachment::Type;

    private:
        EXPORT_RESOURCE_LIST(Pipeline, pipelines);
        EXPORT_RESOURCE_LIST(SubpassAttachment, attachments);

    public:
        void addPipeline(Pipeline* pipeline) { return (*pipelines).push_back(pipeline); }
        void addAttachment(const std::string& imageID, const SubpassType attachmentType)
        {
            SubpassAttachment* elem = *(*attachments).emplace_back();
            elem->setImageID(imageID);
            elem->setAttachmentType(attachmentType);
        }

        [[nodiscard]] bool hasDepthAttachment() const;
        const std::vector<Pipeline*>& getPipelines() { return (*pipelines).data(); }

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

    inline bool RenderPassSubpass::hasDepthAttachment() const
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
