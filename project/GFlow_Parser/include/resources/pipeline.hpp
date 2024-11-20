#pragma once
#include <filesystem>

#include "../resource_manager.hpp"

#include "list.hpp"
#include "vulkan_shader.hpp"


namespace gflow::parser
{
    class PipelineInputAssemblyState final : public Resource
    {
        EXPORT_ENUM(topology, EnumContexts::primitiveTopology);
        EXPORT(bool, restartEnable);
        EXPORT(std::string, something);
        
    public:
        DECLARE_PUBLIC_RESOURCE(PipelineInputAssemblyState)
    };

    class PipelineRasterizationState final : public Resource
    {
        EXPORT_ENUM(polygonMode, EnumContexts::polygonMode);
        EXPORT_ENUM(cullMode, EnumContexts::cullMode);
        EXPORT_ENUM(frontFace, EnumContexts::frontFace);
        
    public:
        DECLARE_PUBLIC_RESOURCE(PipelineRasterizationState)
    };

    class PipelineDepthStencilState final : public Resource
    {
        EXPORT(bool, depthTestEnable);
        EXPORT(bool, stencilTestEnable);
        EXPORT_ENUM(depthCompareOp, EnumContexts::compareOp);
        
    public:
        DECLARE_PUBLIC_RESOURCE(PipelineDepthStencilState)
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
        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;
        
    public:
        DECLARE_PUBLIC_RESOURCE(PipelineColorBlendAttachment)

        template <typename T>
        friend class List;
    };

    class PipelineColorBlendState final : public Resource
    {
        EXPORT(bool, logicOpEnable);
        EXPORT_ENUM(logicOp, EnumContexts::blendOp);
        EXPORT(Vec4, colorBlendConstants);
        EXPORT_RESOURCE_LIST(PipelineColorBlendAttachment, colorBlendAttachments);

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;
        
    public:
        DECLARE_PUBLIC_RESOURCE(PipelineColorBlendState)
    };

    class Pipeline final : public Resource
    {
    public:
        void exportChanged(const std::string& variable) override;

    private:
        EXPORT_RESOURCE(PipelineInputAssemblyState, inputAssemblyState);
        EXPORT_RESOURCE(PipelineRasterizationState, rasterizationState);
        EXPORT_RESOURCE(PipelineDepthStencilState, depthStencilState);
        EXPORT_RESOURCE(PipelineColorBlendState, colorBlendState);
        EXPORT_GROUP(shaders, "Shaders");
        EXPORT(FilePath, vertex);
        EXPORT(FilePath, fragment);
        
    public:
        enum ShaderStage { VERTEX, FRAGMENT };
        VulkanShader::ReflectionData getShaderReflectionData(ShaderStage type, bool forceRecalculation = false);

        VulkanShader::ReflectionData m_vertexReflection;
        VulkanShader::ReflectionData m_fragmentReflection;

        DECLARE_PUBLIC_RESOURCE(Pipeline)
    };

    // ********************
    // Function definitions
    // ********************

    inline DataUsage PipelineColorBlendAttachment::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (parentPath.size() > 1 && parentPath[parentPath.size() - 2]->getValue<bool>("logicOpEnable"))
            return variable == "colorWriteMask" ? USED : NOT_USED;
        if (variable == "blendEnable")
            return USED;
        return *blendEnable ? USED : NOT_USED;
    }

    inline DataUsage PipelineColorBlendState::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "logicOpEnable" || variable == "colorBlendAttachments")
            return USED;
        return *logicOpEnable ? USED : NOT_USED;
    }

    inline void Pipeline::exportChanged(const std::string& variable)
    {
        if (variable == "vertex")
            getShaderReflectionData(VERTEX, true);
        else if (variable == "fragment")
            getShaderReflectionData(FRAGMENT, true);
        Resource::exportChanged(variable);
    }

    inline VulkanShader::ReflectionData Pipeline::getShaderReflectionData(const ShaderStage type, const bool forceRecalculation)
    {
        switch (type)
        {
        case VERTEX:
            {
                if (!(*vertex).path.empty() && m_vertexReflection.valid && !forceRecalculation)
                    return m_vertexReflection;
                m_vertexReflection = VulkanShader::getReflectionDataFromFile(
                    ResourceManager::makePathAbsolute((*vertex).path),
                    VK_SHADER_STAGE_VERTEX_BIT);
            }
            return m_vertexReflection;
        case FRAGMENT:
            {
                if (!(*fragment).path.empty() && m_fragmentReflection.valid && !forceRecalculation)
                    return m_fragmentReflection;
                m_fragmentReflection = VulkanShader::getReflectionDataFromFile(
                    ResourceManager::makePathAbsolute((*fragment).path), 
                    VK_SHADER_STAGE_FRAGMENT_BIT);
            }
            return m_fragmentReflection;
        }
        return {};
    }
}
