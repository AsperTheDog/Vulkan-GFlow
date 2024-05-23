#pragma once
#include "resource.hpp"
#include "list.hpp"
#include "vertex_binding.hpp"


namespace gflow::parser
{
    class PipelineVertexInputState final : public Resource
    {
        EXPORT_RESOURCE_LIST(VertexBinding, vertexBindings);

        explicit PipelineVertexInputState(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(VertexInputState)
    };

    class PipelineInputAssemblyState final : public Resource
    {
        EXPORT_ENUM(topology, EnumContexts::primitiveTopology);
        EXPORT(bool, restartEnable);

        explicit PipelineInputAssemblyState(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(PipelineInputAssemblyState)
    };

    class PipelineRasterizationState final : public Resource
    {
        EXPORT_ENUM(polygonMode, EnumContexts::polygonMode);
        EXPORT_ENUM(cullMode, EnumContexts::cullMode);
        EXPORT_ENUM(frontFace, EnumContexts::frontFace);

        explicit PipelineRasterizationState(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(PipelineRasterizationState)
    };

    class PipelineDepthStencilState final : public Resource
    {
        EXPORT(bool, depthTestEnable);
        EXPORT(bool, stencilTestEnable);
        EXPORT_ENUM(depthCompareOp, EnumContexts::compareOp);

        explicit PipelineDepthStencilState(const std::string& path) : Resource(path) {}

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

        explicit PipelineColorBlendAttachment(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(PipelineColorBlendAttachment)

        template <typename T>
        friend class List;
    };

    class PipelineColorBlendState final : public Resource
    {
        EXPORT(bool, logicOpEnable);
        EXPORT_ENUM(logicOp, EnumContexts::blendOp);
        EXPORT_RESOURCE_LIST(PipelineColorBlendAttachment, colorBlendAttachments);
        EXPORT(Vec4, colorBlendConstants);

        explicit PipelineColorBlendState(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(PipelineColorBlendState)
    };

    class Pipeline final : public Resource
    {
    private:
        EXPORT_RESOURCE(PipelineVertexInputState, vertexInputState);
        EXPORT_RESOURCE(PipelineInputAssemblyState, inputAssemblyState);
        EXPORT_RESOURCE(PipelineRasterizationState, rasterizationState);
        EXPORT_RESOURCE(PipelineDepthStencilState, depthStencilState);
        EXPORT_RESOURCE(PipelineColorBlendState, colorBlendState);

        explicit Pipeline(const std::string& path) : Resource(path) {}

        DECLARE_RESOURCE(Pipeline)
    };

    inline Resource* PipelineVertexInputState::create(const std::string& path, const ExportData* metadata)
    {
        PipelineVertexInputState* vertexInputState = new PipelineVertexInputState(path);
        if (!vertexInputState->deserialize())
            vertexInputState->serialize();
        return vertexInputState;
    }

    inline Resource* PipelineInputAssemblyState::create(const std::string& path, const ExportData* metadata)
    {
        PipelineInputAssemblyState* inputAssemblyState = new PipelineInputAssemblyState(path);
        if (!inputAssemblyState->deserialize())
            inputAssemblyState->serialize();
        return inputAssemblyState;
    }

    inline Resource* PipelineRasterizationState::create(const std::string& path, const ExportData* metadata)
    {
        PipelineRasterizationState* rasterizationState = new PipelineRasterizationState(path);
        if (!rasterizationState->deserialize())
            rasterizationState->serialize();
        return rasterizationState;
    }

    inline Resource* PipelineDepthStencilState::create(const std::string& path, const ExportData* metadata)
    {
        PipelineDepthStencilState* depthStencilState = new PipelineDepthStencilState(path);
        if (!depthStencilState->deserialize())
            depthStencilState->serialize();
        return depthStencilState;
    }

    inline Resource* PipelineColorBlendAttachment::create(const std::string& path, const ExportData* metadata)
    {
        PipelineColorBlendAttachment* colorBlendAttachment = new PipelineColorBlendAttachment(path);
        if (!colorBlendAttachment->deserialize())
            colorBlendAttachment->serialize();
        return colorBlendAttachment;
    }

    inline Resource* PipelineColorBlendState::create(const std::string& path, const ExportData* metadata)
    {
        PipelineColorBlendState* colorBlendState = new PipelineColorBlendState(path);
        if (!colorBlendState->deserialize())
            colorBlendState->serialize();
        return colorBlendState;
    }

    inline Resource* Pipeline::create(const std::string& path, const ExportData* metadata)
    {
        Pipeline* pipeline = new Pipeline(path);
        if (!pipeline->deserialize())
            pipeline->serialize();
        return pipeline;
    }
}
