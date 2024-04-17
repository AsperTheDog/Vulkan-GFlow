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
    gflow::Environment& env = gflow::Context::getEnvironment(environment);
    env.addSurface(window.getSurface());
	{
		gflow::Project::Requirements requirements{};
    	requirements.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    	requirements.present = true;
    	requirements.extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    	env.manualBuild(requirements);
	}

    env.configurePresentTarget(window.getSurface(), window.getSize().toExtent2D(), {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});

    Logger::setRootContext("Environment cleanup");
    gflow::Context::destroyEnvironment(environment);

    window.free();
    gflow::Context::destroy();
    return 0;
}
