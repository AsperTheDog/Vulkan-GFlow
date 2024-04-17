#include "context.hpp"
#include "sdl_window.hpp"
#include "utils/logger.hpp"

int main(int argc, char *argv[])
{
    Logger::setLevels(Logger::ALL);
    Logger::setRootContext("Window init");

    SDLWindow window{"GFlow", 1920, 1080};

    Logger::setRootContext("Vulkan init");
    gflow::Context::initVulkan(window.getRequiredVulkanExtensions());
    window.createSurface(gflow::Context::getVulkanInstance());

    Logger::setRootContext("Environment init");
    const uint32_t environment = gflow::Context::createEnvironment();
    gflow::Project::Requirements requirements{};
    requirements.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    requirements.present = true;
    requirements.extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    gflow::Context::getEnvironment(environment).manualBuild(requirements);

    Logger::setRootContext("Environment cleanup");
    gflow::Context::destroyEnvironment(environment);
    return 0;
}
