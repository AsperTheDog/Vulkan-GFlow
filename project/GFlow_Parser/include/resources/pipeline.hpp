#pragma once
#include "../resource_manager.hpp"

#include "list.hpp"
#include "vulkan_shader.hpp"
#include "utils/shader_reflection.hpp"


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

        void updateShaderReflectionData(ShaderStage p_Type, bool p_ForceRecalculation = false);
        ShaderReflectionData getShaderReflectionData(ShaderStage p_Type);
        void exportChanged(const std::string& variable) override;
        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;

        struct ShaderStructure
        {
            enum class PrimitiveType
            {
                INT, UINT, FLOAT, BOOL,
                VEC2, VEC3, VEC4,
                MAT2, MAT3, MAT4,
                BUFFER, IMAGE,
            };

            struct Member
            {
                std::string name;
                PrimitiveType type;
            };

            std::vector<Member> members;
        };

        VulkanShader m_VertexShader{};
        VulkanShader m_FragmentShader{};

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

    inline void Pipeline::updateShaderReflectionData(const ShaderStage p_Type, const bool p_ForceRecalculation)
    {
        try
        {
            switch (p_Type)
            {
            case VERTEX:
                if ((*vertex).path.empty())
                {
                    VulkanShader::reset(m_VertexShader);
                    return;
                }
                if (m_VertexShader.getStatus().status != VulkanShader::Result::COMPILED || p_ForceRecalculation)
                {
                    VulkanShader::reinit(m_VertexShader, 0, false);
                    m_VertexShader.loadModule(ResourceManager::makePathAbsolute((*vertex).path), "main");
                    m_VertexShader.linkAndFinalize();
                    if (m_VertexShader.getStatus().status == VulkanShader::Result::COMPILED)
                        LOG_INFO("Vertex shader for ", (*vertex).path, " compiled successfully");
                    else 
                        throw std::runtime_error("Vertex shader compilation failed for " + (*vertex).path);
                }
                break;
            case FRAGMENT:
                if ((*fragment).path.empty())
                {
                    VulkanShader::reset(m_FragmentShader);
                    return;
                }
                if (m_FragmentShader.getStatus().status != VulkanShader::Result::COMPILED || p_ForceRecalculation)
                {
                    VulkanShader::reinit(m_FragmentShader, 0, false);
                    m_FragmentShader.loadModule(ResourceManager::makePathAbsolute((*fragment).path), "main");
                    m_FragmentShader.linkAndFinalize();
                    if (m_FragmentShader.getStatus().status == VulkanShader::Result::COMPILED)
                        LOG_INFO("Fragment shader for ", (*fragment).path, " compiled successfully");
                    else 
                        throw std::runtime_error("Fragment shader compilation failed for " + (*fragment).path);
                }
                break;
            }
        }
        catch (const std::runtime_error& e)
        {
            Logger::print(Logger::WARN, e.what());
        }
    }

    inline ShaderReflectionData Pipeline::getShaderReflectionData(const ShaderStage p_Type)
    {
        updateShaderReflectionData(p_Type);
        switch (p_Type)
        {
        case VERTEX:
            if (m_VertexShader.getStatus().status == VulkanShader::Result::COMPILED)
            {
                return { m_VertexShader.getLayout(), VK_SHADER_STAGE_VERTEX_BIT };
            }
            break;
        case FRAGMENT:
            if (m_FragmentShader.getStatus().status == VulkanShader::Result::COMPILED)
            {
                return { m_FragmentShader.getLayout(), VK_SHADER_STAGE_FRAGMENT_BIT };
            }
            break;
        }

        return {};
    }

    inline void Pipeline::exportChanged(const std::string& variable)
    {
        if (variable == "vertex")
            updateShaderReflectionData(VERTEX, true);
        else if (variable == "fragment")
            updateShaderReflectionData(FRAGMENT, true);
        Resource::exportChanged(variable);
    }

    inline DataUsage Pipeline::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (variable == "attachments")
            return NOT_USED;
        return Resource::isUsed(variable, parentPath);
    }
}
