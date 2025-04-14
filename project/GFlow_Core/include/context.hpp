#pragma once
#include <vector>

#include "environment.hpp"

namespace gflow
{
    class Context
    {
    public:
    	static void initVulkan(std::span<const char*> vulkanInstanceExtensions);

        static uint32_t createEnvironment();
        static Environment& getEnvironment(uint32_t id);
        static void destroyEnvironment(uint32_t id);
        static void destroyEnvironment(const Environment& environment);

        static Project& loadProject(const std::string& path, uint32_t gpuOverride = UINT32_MAX);
	    static VkInstance getVulkanInstance();

        static void destroy();

    private:
        inline static std::vector<Environment> m_environments{};
    };
} // namespace gflow