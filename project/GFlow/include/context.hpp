#pragma once
#include <initializer_list>
#include <string_view>
#include <vector>

#include "environment.hpp"

namespace gflow
{
    class Context
    {
    public:
    	static void initVulkan(std::initializer_list<const char*> vulkanInstanceExtensions);

        static uint32_t createEnvironment();
        static Environment& getEnvironment(uint32_t id);
        static void destroyEnvironment(uint32_t id);
        static void destroyEnvironment(const Environment& environment);

        static uint32_t loadProject(std::string_view path, uint32_t gpuOverride = UINT32_MAX);

    private:
        inline static std::vector<Environment> m_environments{};
    };
} // namespace gflow