#pragma once
#include "resource.hpp"
#include "list.hpp"


namespace gflow::parser
{
    class PipelineInputAssemblyState final : public Resource
    {
        EXPORT_ENUM(topology, EnumContexts::primitiveTopology);
        EXPORT(bool, restartEnable);
        
    public:
        DECLARE_RESOURCE(PipelineInputAssemblyState)
    };

    class PipelineRasterizationState final : public Resource
    {
        EXPORT_ENUM(polygonMode, EnumContexts::polygonMode);
        EXPORT_ENUM(cullMode, EnumContexts::cullMode);
        EXPORT_ENUM(frontFace, EnumContexts::frontFace);
        
    public:
        DECLARE_RESOURCE(PipelineRasterizationState)
    };

    class PipelineDepthStencilState final : public Resource
    {
        EXPORT(bool, depthTestEnable);
        EXPORT(bool, stencilTestEnable);
        EXPORT_ENUM(depthCompareOp, EnumContexts::compareOp);
        
    public:
        DECLARE_RESOURCE(PipelineDepthStencilState)
    };

    class PipelineColorBlendAttachment final : public Resource
    {
        EXPORT(bool, blendEnable);
        EXPORT_ENUM(srcColorBlendFactor, EnumContexts::blendFactor);
        EXPORT_ENUM(dstColorBlendFactor, EnumContexts::blendFactor);
        EXPORT_ENUM(colorBlendOp, EnumContexts::blendOp);
        EXPORT_ENUM(srcAlphaBlendFactor, EnumContexts::blendFactor);
        EXPORT_ENUM(dstAlphaBlendFactor, EnumContexts::blendFactor);
        EXPORT_ENUM(alphaBlendOp, EnumContexts::blendOp);
        EXPORT_BITMASK(colorWriteMask, EnumContexts::colorWriteMaskBits);

        void initContext(ExportData* metadata) override {}
        bool isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;
        
    public:
        DECLARE_RESOURCE(PipelineColorBlendAttachment)

        template <typename T>
        friend class List;
    };

    class PipelineColorBlendState final : public Resource
    {
        EXPORT(bool, logicOpEnable);
        EXPORT_ENUM(logicOp, EnumContexts::blendOp);
        EXPORT(Vec4, colorBlendConstants);
        EXPORT_RESOURCE_LIST(PipelineColorBlendAttachment, colorBlendAttachments);

        bool isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;
        
    public:
        DECLARE_RESOURCE(PipelineColorBlendState)
    };

    class Pipeline final : public Resource
    {
    private:
        EXPORT_RESOURCE(PipelineInputAssemblyState, inputAssemblyState);
        EXPORT_RESOURCE(PipelineRasterizationState, rasterizationState);
        EXPORT_RESOURCE(PipelineDepthStencilState, depthStencilState);
        EXPORT_RESOURCE(PipelineColorBlendState, colorBlendState);
        
    public:
        DECLARE_RESOURCE(Pipeline)
    };

    // ********************
    // Function definitions
    // ********************

    inline bool PipelineColorBlendAttachment::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (parentPath.size() > 1 && parentPath[parentPath.size() - 2]->getValue<bool>("logicOpEnable"))
            return variable == "colorWriteMask";
        if (variable == "blendEnable")
            return true;
        return *blendEnable;
    }

    inline bool PipelineColorBlendState::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "logicOpEnable" || variable == "colorBlendAttachments")
            return true;
        return *logicOpEnable;
    }
}
