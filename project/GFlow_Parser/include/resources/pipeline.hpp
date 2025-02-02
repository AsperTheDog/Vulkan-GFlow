#pragma once
#include "../resource_manager.hpp"

#include "list.hpp"
#include "vulkan_shader.hpp"


namespace gflow::parser
{
    class PipelineInputAssemblyState final : public Resource
    {
        EXPORT_ENUM(topology, EnumContexts::primitiveTopology);
        EXPORT(bool, restartEnable);
        
    public:
        DECLARE_PRIVATE_RESOURCE(PipelineInputAssemblyState)
    };

    class PipelineRasterizationState final : public Resource
    {
        EXPORT_ENUM(polygonMode, EnumContexts::polygonMode);
        EXPORT_ENUM(cullMode, EnumContexts::cullMode);
        EXPORT_ENUM(frontFace, EnumContexts::frontFace);
        
    public:
        DECLARE_PRIVATE_RESOURCE(PipelineRasterizationState)
    };

    class PipelineDepthStencilState final : public Resource
    {
        EXPORT(bool, depthTestEnable);
        EXPORT(bool, stencilTestEnable);
        EXPORT_ENUM(depthCompareOp, EnumContexts::compareOp);
        
    public:
        DECLARE_PRIVATE_RESOURCE(PipelineDepthStencilState)
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

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;
        
    public:
        DECLARE_PRIVATE_RESOURCE(PipelineColorBlendAttachment)

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
        DECLARE_PRIVATE_RESOURCE(PipelineColorBlendState)
    };

    class Pipeline final : public Resource
    {
    private:
        EXPORT_RESOURCE(PipelineInputAssemblyState, inputAssemblyState, true, false);
        EXPORT_RESOURCE(PipelineRasterizationState, rasterizationState, true, false);
        EXPORT_RESOURCE(PipelineDepthStencilState, depthStencilState, true, false);
        EXPORT_RESOURCE(PipelineColorBlendState, colorBlendState, true, false);
        EXPORT_GROUP(shaders, "Shaders");
        EXPORT(FilePath, vertex);
        EXPORT(FilePath, fragment);
        
    public:
        enum ShaderStage : uint8_t { VERTEX, FRAGMENT };

        VulkanShader::ReflectionManager* getShaderReflectionData(ShaderStage type, bool forceRecalculation = false);
        void exportChanged(const std::string& variable) override;
        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;

        VulkanShader::ReflectionManager m_vertexReflection;
        VulkanShader::ReflectionManager m_fragmentReflection;

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

    inline DataUsage Pipeline::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "attachments")
            return NOT_USED;
        return Resource::isUsed(variable, parentPath);
    }

    inline VulkanShader::ReflectionManager* Pipeline::getShaderReflectionData(
        const ShaderStage type, const bool forceRecalculation)
    {
        try
        {
            switch (type)
            {
            case VERTEX:
                if ((*vertex).path.empty())
                    m_vertexReflection = {};
                else if (!m_vertexReflection.isValid() || forceRecalculation)
                    m_vertexReflection = VulkanShader::getReflectionDataFromFile(ResourceManager::makePathAbsolute((*vertex).path), VK_SHADER_STAGE_VERTEX_BIT);
                return &m_vertexReflection;
            case FRAGMENT:
                if ((*fragment).path.empty())
                    m_fragmentReflection = {};
                else if (!m_fragmentReflection.isValid() || forceRecalculation)
                    m_fragmentReflection = VulkanShader::getReflectionDataFromFile(ResourceManager::makePathAbsolute((*fragment).path), VK_SHADER_STAGE_FRAGMENT_BIT);
                return &m_fragmentReflection;
            }
        }
        catch (const std::runtime_error& e)
        {
            Logger::print(Logger::WARN, e.what());
        }
        return nullptr;
    }
}
