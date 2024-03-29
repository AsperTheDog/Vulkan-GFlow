#include "context.hpp"

#include "config_module.hpp"
#include "vulkan_context.hpp"
#include "utils/logger.hpp"

namespace gflow
{
    void Context::initVulkan(std::initializer_list<const char*> vulkanInstanceExtensions)
    {
#ifndef _DEBUG
        VulkanContext::init(VK_API_VERSION_1_0, true, true, vulkanInstanceExtensions);
#else
        VulkanContext::init(VK_API_VERSION_1_0, true, true, vulkanInstanceExtensions);
#endif
    }

    void Context::setConfig(const std::string_view filepath)
    {
        m_config = ConfigModule::load(filepath);
    }

    void Context::setConfig(const ConfigModule& config)
    {
        m_config = config;
    }

    Project Context::loadProject(const std::string_view path)
    {
        return Project::load(path);
    }
} // namespace gflow
