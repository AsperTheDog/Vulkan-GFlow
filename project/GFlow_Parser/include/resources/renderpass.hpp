#pragma once
#include "../resource_manager.hpp"

#include "pair.hpp"
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

    class RenderPassPipeline final : public Resource
    {
        using StrPair = Pair<std::string, std::string>;

    private:
        EXPORT_RESOURCE(Pipeline, pipeline, false, true);

        EXPORT_RESOURCE_LIST(StrPair, pushConstants);
        EXPORT_RESOURCE_LIST(StrPair, attachments);

    public:
        Pipeline* getPipeline() { return *pipeline; }
        void setPipeline(Pipeline* pipeline) { this->pipeline.setData(pipeline); }

        DECLARE_PRIVATE_RESOURCE(RenderPassPipeline)

        template <typename T>
        friend class List;
    };

    class RenderPassSubpass final : public Resource
    {
        using SubpassType = SubpassAttachment::Type;

    private:
        EXPORT_RESOURCE_LIST(RenderPassPipeline, pipelines);
        EXPORT_RESOURCE_LIST(SubpassAttachment, attachments);

    public:
        RenderPassPipeline* addPipeline() { return *(*pipelines).emplace_back(); }
        void addAttachment(const std::string& imageID, const SubpassType attachmentType)
        {
            SubpassAttachment* elem = *(*attachments).emplace_back();
            elem->setImageID(imageID);
            elem->setAttachmentType(attachmentType);
        }

        [[nodiscard]] bool hasDepthAttachment() const;
        const std::vector<RenderPassPipeline*>& getPipelines() { return (*pipelines).data(); }

        DECLARE_PRIVATE_RESOURCE(RenderPassSubpass)

        template <typename T>
        friend class List;
    };

    class RenderPassCustomDependency final : public Resource
    {
        EXPORT(int, srcSubpass);
        EXPORT(int, destSubpass);

    public:
        DECLARE_PRIVATE_RESOURCE(RenderPassCustomDependency)

        template <typename T>
        friend class List;
    };

    class PushConstantElement final : public Resource
    {
        EXPORT(std::string, name);
        EXPORT_ENUM(type, EnumContexts::PushConstantElement);

    public:
        DECLARE_PRIVATE_RESOURCE(PushConstantElement)

        template <typename T>
        friend class List;
    };

    class PushConstantStructure final : public Resource
    {
        EXPORT(std::string, structureID);
        EXPORT_RESOURCE_LIST(PushConstantElement, elements);
        EXPORT_GROUP(g_data, "Data");
        EXPORT(bool, external);
        EXPORT(FilePath, data);


        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    public:
        std::vector<std::string> getElementNames() const;

        DECLARE_PRIVATE_RESOURCE(PushConstantStructure)

        template <typename T>
        friend class List;
    };

    class ImageAttachment final : public Resource
    {
        EXPORT(std::string, imageID);
        EXPORT(bool, clear);

    public:
        DECLARE_PUBLIC_RESOURCE(ImageAttachment)

        template <typename T>
        friend class List;
    };

    class RenderPass final : public Resource
    {
        EXPORT_RESOURCE_LIST(ImageAttachment, attachments);
        EXPORT_RESOURCE_LIST(RenderPassCustomDependency, customDependencies);
        EXPORT_RESOURCE_LIST(PushConstantStructure, pushConstants);

        EXPORT_RESOURCE_LIST(RenderPassSubpass, subpasses);

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;

    public:
        void clearSubpasses() { (*subpasses).clear(); }
        RenderPassSubpass* addSubpass() { return *(*subpasses).emplace_back(); }

        std::vector<std::string> getAttachmentIDs() const;
        std::vector<std::string> getPushConstantIDs(bool includeInternal) const;

        PushConstantStructure* getPushConstantStructure(const std::string& structureID);

        DECLARE_PUBLIC_RESOURCE(RenderPass)
    };

    inline bool RenderPassSubpass::hasDepthAttachment() const
    {
        for (RenderPassPipeline* pipeline : (*pipelines).data())
        {
            PipelineDepthStencilState* depthState = pipeline->getPipeline()->getValue<PipelineDepthStencilState*>("depthStencilState");
            if (depthState != nullptr && depthState->getValue<bool>("depthTestEnable"))
                return true;
        }
        return false;
    }

    inline DataUsage PushConstantStructure::isUsed(const std::string& variable,
        const std::vector<Resource*>& parentPath)
    {
        if (variable == "data")
        {
            if (*external)
                return NOT_USED;
            return USED;
        }
        return USED;
    }

    inline std::vector<std::string> PushConstantStructure::getElementNames() const
    {
        std::vector<std::string> names;
        for (PushConstantElement* element : (*elements).data())
            names.push_back(element->getValue<std::string>("name"));
        return names;
    }

    inline DataUsage RenderPass::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "subpasses")
            return NOT_USED;
        return USED;
    }

    inline std::vector<std::string> RenderPass::getAttachmentIDs() const
    {
        std::vector<std::string> ids;
        for (ImageAttachment* image : (*attachments).data())
            ids.push_back(image->getValue<std::string>("imageID"));
        return ids;
    }

    inline std::vector<std::string> RenderPass::getPushConstantIDs(const bool includeInternal) const
    {
        std::vector<std::string> ids;
        for (PushConstantStructure* pushConstant : (*pushConstants).data())
            if (includeInternal || pushConstant->getValue<bool>("external"))
                ids.push_back(pushConstant->getValue<std::string>("structureID"));
        return ids;
    }

    inline PushConstantStructure* RenderPass::getPushConstantStructure(const std::string& structureID)
    {
        for (PushConstantStructure* pushConstant : (*pushConstants).data())
            if (pushConstant->getValue<std::string>("structureID") == structureID)
                return pushConstant;
        return nullptr;
    }
}
