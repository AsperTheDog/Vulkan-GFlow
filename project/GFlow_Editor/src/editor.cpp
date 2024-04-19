#include "editor.hpp"

#include "context.hpp"
#include "imgui.h"
#include "vulkan_context.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "utils/logger.hpp"

void Editor::init()
{
	Logger::setLevels(Logger::ALL);
    Logger::setRootContext("Window init");

    s_window = SDLWindow{"GFlow", 1280, 720};

    Logger::setRootContext("Vulkan init");
    gflow::Context::initVulkan(s_window.getRequiredVulkanExtensions());
    s_window.createSurface(gflow::Context::getVulkanInstance());

    createEnv();
    initImgui();
}

void Editor::run()
{
    while (!s_window.shouldClose())
    {
	    s_window.pollEvents();
        renderFrame();
    }
}

void Editor::cleanup()
{
    Logger::setRootContext("Environment cleanup");
    VulkanContext::getDevice(gflow::Context::getEnvironment(s_environment).man_getDevice()).waitIdle();
    
	ImGui_ImplVulkan_Shutdown();
	s_window.shutdownImgui();
    ImGui::DestroyContext();

    gflow::Context::destroyEnvironment(s_environment);

    s_window.free();
    gflow::Context::destroy();
}

void Editor::createEnv()
{
    Logger::setRootContext("Environment init");
    s_environment = gflow::Context::createEnvironment();
    gflow::Environment& env = gflow::Context::getEnvironment(s_environment);
    env.addSurface(s_window.getSurface());
	{
		gflow::Project::Requirements requirements{};
    	requirements.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
    	requirements.present = true;
    	requirements.extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    	env.man_manualBuild(requirements);
	}

    env.configurePresentTarget(s_window.getSurface(), s_window.getSize().toExtent2D(), {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
}

void Editor::initImgui()
{
    Logger::pushContext("Init Imgui");
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    
    gflow::Environment& env = gflow::Context::getEnvironment(s_environment);
	VulkanDevice& device = VulkanContext::getDevice(env.man_getDevice());

	const std::vector<VkDescriptorPoolSize> pool_sizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

	const uint32_t imguiPoolID = device.createDescriptorPool(pool_sizes, 1000U * pool_sizes.size(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
    const VulkanSwapchain& swapchain = device.getSwapchain(env.man_getSwapchain(s_window.getSurface()));

	{
        Logger::pushContext("Create Imgui Renderpass");
    	VulkanRenderPassBuilder builder{};
		const VkAttachmentDescription colorAttachment = VulkanRenderPassBuilder::createAttachment(swapchain.getFormat().format, 
			VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE, 
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    	builder.addAttachment(colorAttachment);
    	builder.addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, {{COLOR, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}}, 0);
    	VkSubpassDependency dependency{};
    	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    	dependency.dstSubpass = 0;
    	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    	dependency.srcAccessMask = 0;
    	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    	builder.addDependency(dependency);
    	s_imguiRenderPass = device.createRenderPass(builder, 0);
        Logger::popContext();
	}

    const VulkanRenderPass& renderPass = device.getRenderPass(s_imguiRenderPass);

    s_window.initImgui();
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VulkanContext::getHandle();
    init_info.PhysicalDevice = *device.getGPU();
    init_info.Device = *device;
    init_info.QueueFamily = env.man_getQueuePos(QueueFamilyTypeBits::GRAPHICS).familyIndex;
    init_info.Queue = *device.getQueue(env.m_mainQueue);
    init_info.DescriptorPool = *device.getDescriptorPool(imguiPoolID);
    init_info.RenderPass = *renderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = swapchain.getMinImageCount();
    init_info.ImageCount = swapchain.getImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info);
    Logger::popContext();

    const VkExtent3D extent = {swapchain.getExtent().width, swapchain.getExtent().height, 1};
    for (uint32_t i = 0; i < swapchain.getImageCount(); ++i)
    	s_imguiFrameBuffers.push_back(device.createFramebuffer(extent, renderPass, {swapchain.getImageView(i)}));
}

void Editor::renderFrame()
{
	gflow::Environment& env = gflow::Context::getEnvironment(s_environment);

    env.beginRecording({s_window.getSurface()});

    if (!renderImgui())
	{
		updateImguiWindows();
        return;
	}
    env.setRecordingBarrier();
    submitImgui();
    env.endRecording();
    updateImguiWindows();
    env.present(s_window.getSurface());
}

bool Editor::renderImgui()
{
    ImGui_ImplVulkan_NewFrame();
    s_window.frameImgui();
    ImGui::NewFrame();

	drawImgui();

	ImGui::Render();

    const ImDrawData* imguiDrawData = ImGui::GetDrawData();
	if (imguiDrawData->DisplaySize.x <= 0.0f || imguiDrawData->DisplaySize.y <= 0.0f)
	{
        return false;
	}
    return true;
}

void Editor::submitImgui()
{
	gflow::Environment& env = gflow::Context::getEnvironment(s_environment);
	ImDrawData* imguiDrawData = ImGui::GetDrawData();

	const VulkanCommandBuffer& commandBuffer = VulkanContext::getDevice(env.man_getDevice()).getCommandBuffer(env.man_getCommandBuffer(), 0);

	const std::vector<VkClearValue> clearValues{{0.0f, 0.0f, 0.0f, 1.0f}};

	const uint32_t frameBuffer = s_imguiFrameBuffers[env.man_getSwapchainImage(s_window.getSurface())];

	commandBuffer.cmdBeginRenderPass(s_imguiRenderPass, frameBuffer, s_window.getSize().toExtent2D(), clearValues);
		ImGui_ImplVulkan_RenderDrawData(imguiDrawData, *commandBuffer);
    commandBuffer.cmdEndRenderPass();
}

void Editor::drawImgui()
{
	ImGui::ShowDemoWindow();
}

void Editor::updateImguiWindows()
{
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}
