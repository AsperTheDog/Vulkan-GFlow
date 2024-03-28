#include "context.hpp"

#include <vulkan_context.hpp>

namespace gflow
{
    void Context::init()
    {
        VulkanContext::init(VK_API_VERSION_1_3, true, true, {});
    }
}
