#pragma once
#include <initializer_list>

#include "config_module.hpp"
#include "project.hpp"

namespace gflow
{
    class Context
    {
    public:
        static void initVulkan(std::initializer_list<const char*> vulkanInstanceExtensions);
        static void setConfig(std::string_view filepath);
        static void setConfig(const ConfigModule& config);

        static Project loadProject(std::string_view path);

    private:
        inline static ConfigModule m_config{};
    };
} // namespace gflow