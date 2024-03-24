#include "engine.hpp"

#include <stdexcept>

#include <imgui.h>
#include <glm/vec3.hpp>

#include "logger.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "gui/gui_manager.hpp"
#include "VkBase/vulkan_context.hpp"

enum
{
	WINDOW_WIDTH = 1920,
	WINDOW_HEIGHT = 1080
};

constexpr float toFloat(const uint32_t value)
{
	return static_cast<float>(value);
}

VulkanGPU chooseCorrectGPU()
{
	const std::vector<VulkanGPU> gpus = VulkanContext::getGPUs();
	for (auto& gpu : gpus)
	{
		if (gpu.getProperties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return gpu;
		}
	}
	throw std::runtime_error("No discrete GPU found");
}

Engine::Engine()
	: cam({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 1.0f}, toFloat(WINDOW_WIDTH) / toFloat(WINDOW_HEIGHT)), m_window("Vulkan", WINDOW_WIDTH, 1080)
{
	Logger::setRootContext("Engine init");
#ifndef _DEBUG
	VulkanContext::init(VK_API_VERSION_1_3, false, m_window.getRequiredVulkanExtensions());
#else
	VulkanContext::init(VK_API_VERSION_1_3, true, m_window.getRequiredVulkanExtensions());
#endif
	m_window.createSurface();
	createDevice();
	createSwapchain();
	createSyncObjects();
	createRenderPass();

	createSwapchainFramebuffers(false);

	setupEvents();
	initImgui();
}

Engine::~Engine()
{
	VulkanContext::getDevice(m_deviceID).waitIdle();

	Logger::setRootContext("Resource cleanup");

	ImGui_ImplVulkan_Shutdown();
	m_window.shutdownImgui();
    ImGui::DestroyContext();

	m_window.free();
	VulkanContext::free();
}

void Engine::run()
{
	VulkanDevice& device = VulkanContext::getDevice(m_deviceID);
	VulkanFence& inFlightFence = device.getFence(m_inFlightFenceID);

	const VulkanQueue graphicsQueue = device.getQueue(m_graphicsQueuePos);
	const VulkanQueue presentQueue = device.getQueue(m_presentQueuePos);
	const VulkanCommandBuffer& graphicsBuffer = device.getCommandBuffer(m_graphicsCmdBufferID, 0);

	uint64_t frameCounter = 0;

	while (!m_window.shouldClose())
	{
		Logger::setRootContext("Frame" + std::to_string(frameCounter));

		inFlightFence.wait();
		m_window.pollEvents();

		const uint32_t nextImage = m_window.acquireNextImage(m_imageAvailableSemaphoreID, nullptr);

		inFlightFence.reset();

		if (nextImage == UINT32_MAX)
		{
			frameCounter++;
			continue;
		}

		ImGui_ImplVulkan_NewFrame();
	    m_window.frameImgui();
	    ImGui::NewFrame();

		GUIManager::render();

		ImDrawData* imguiDrawData = ImGui::GetDrawData();
		if (imguiDrawData->DisplaySize.x <= 0.0f || imguiDrawData->DisplaySize.y <= 0.0f)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			continue;
		}
		recordCommandBuffer(m_framebuffers[nextImage], imguiDrawData);

		graphicsBuffer.submit(graphicsQueue, {{m_imageAvailableSemaphoreID, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}}, {m_renderFinishedSemaphoreID}, m_inFlightFenceID);

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		m_window.present(presentQueue, nextImage, m_renderFinishedSemaphoreID);

		frameCounter++;
	}
}

void Engine::createDevice()
{
	const VulkanGPU gpu = chooseCorrectGPU();
	const GPUQueueStructure queueStructure = gpu.getQueueFamilies();
	QueueFamilySelector queueFamilySelector(queueStructure);

	// Select Queue Families and assign queues
	const QueueFamily graphicsQueueFamily = queueStructure.findQueueFamily(VK_QUEUE_GRAPHICS_BIT);
	const QueueFamily presentQueueFamily = queueStructure.findPresentQueueFamily(m_window.getSurface());
	const QueueFamily transferQueueFamily = queueStructure.findQueueFamily(VK_QUEUE_TRANSFER_BIT);

	QueueFamilySelector selector{queueStructure};
	selector.selectQueueFamily(graphicsQueueFamily, QueueFamilyTypeBits::GRAPHICS);
	selector.selectQueueFamily(presentQueueFamily, QueueFamilyTypeBits::PRESENT);
	selector.selectQueueFamily(transferQueueFamily, QueueFamilyTypeBits::TRANSFER);
	m_graphicsImguiQueuePos = selector.getOrAddQueue(graphicsQueueFamily, 0.9f);
	m_graphicsQueuePos = selector.addQueue(graphicsQueueFamily, 1.0f);
	m_presentQueuePos = selector.getOrAddQueue(presentQueueFamily, 0.9f);
	m_transferQueuePos = selector.addQueue(transferQueueFamily, 1.0f);

	m_deviceID = VulkanContext::createDevice(gpu, selector, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, {});
	VulkanContext::getDevice(m_deviceID).configureOneTimeQueue(m_transferQueuePos);
	
	m_graphicsCmdBufferID = VulkanContext::getDevice(m_deviceID).createCommandBuffer(graphicsQueueFamily, 0, false);
}

void Engine::createSwapchain()
{
	m_window.createSwapchain(m_deviceID, {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
	m_framebuffers.resize(m_window.getImageCount());
}

void Engine::createSyncObjects()
{
	VulkanDevice& device = VulkanContext::getDevice(m_deviceID);
	m_imageAvailableSemaphoreID = device.createSemaphore();
	m_renderFinishedSemaphoreID = device.createSemaphore();
	m_inFlightFenceID = device.createFence(true);
}

void Engine::createRenderPass()
{
	VulkanRenderPassBuilder builder{};
	const VkAttachmentDescription colorAttachment = VulkanRenderPassBuilder::createAttachment(
		m_window.getSwapchainImageFormat().format, 
		VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	builder.addAttachment(colorAttachment);

	builder.addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, {{COLOR, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}}, 0);

	m_imguiRenderPassID = VulkanContext::getDevice(m_deviceID).createRenderPass(builder, 0);
}

void Engine::createSwapchainFramebuffers(const bool destroy)
{
	for (uint32_t i = 0; i < m_window.getImageCount(); i++)
	{
		if (destroy)
		{
			VulkanContext::getDevice(m_deviceID).freeFramebuffer(m_framebuffers[i]);
		}
		const VkExtent3D extent = {m_window.getSwapchainExtent().width, m_window.getSwapchainExtent().height, 1};
		std::vector attachments = {m_window.getImageView(i)};
		m_framebuffers[i] = VulkanContext::getDevice(m_deviceID).createFramebuffer(extent, m_imguiRenderPassID, attachments);
	}
}

void Engine::recordCommandBuffer(const uint32_t framebuffer, ImDrawData* im_draw_data) const
{
	Logger::pushContext("Command buffer recording");

	std::vector<VkClearValue> clearValues{2};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

	VulkanCommandBuffer& graphicsBuffer = VulkanContext::getDevice(m_deviceID).getCommandBuffer(m_graphicsCmdBufferID, 0);
	graphicsBuffer.reset();
	graphicsBuffer.beginRecording();

	graphicsBuffer.cmdBeginRenderPass(m_imguiRenderPassID, framebuffer, m_window.getSwapchainExtent(), clearValues);

		ImGui_ImplVulkan_RenderDrawData(im_draw_data, *graphicsBuffer);

	graphicsBuffer.cmdEndRenderPass();
	graphicsBuffer.endRecording();

	Logger::popContext();
}

void Engine::recreateSwapchainResources(const VkExtent2D newExtent)
{
	Logger::pushContext("Swapchain resources rebuild");
	createSwapchainFramebuffers(true);
	Logger::popContext();
}

void Engine::initImgui() const
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
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

	VulkanDevice& device = VulkanContext::getDevice(m_deviceID);

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
	const uint32_t imguiPoolID = device.createDescriptorPool(pool_sizes, 1000U * static_cast<uint32_t>(pool_sizes.size()), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

    m_window.initImgui();
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VulkanContext::getHandle();
    init_info.PhysicalDevice = *device.getGPU();
    init_info.Device = *device;
    init_info.QueueFamily = m_graphicsImguiQueuePos.familyIndex;
    init_info.Queue = *device.getQueue(m_graphicsImguiQueuePos);
    init_info.DescriptorPool = *device.getDescriptorPool(imguiPoolID);
    init_info.RenderPass = *device.getRenderPass(m_imguiRenderPassID);
    init_info.Subpass = 0;
    init_info.MinImageCount = m_window.getMinImageCount();
    init_info.ImageCount = m_window.getImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info);
}

void Engine::setupEvents()
{
	m_window.getMouseMovedSignal().connect(&cam, &Camera::mouseMoved);
	m_window.getKeyPressedSignal().connect(&cam, &Camera::keyPressed);
	m_window.getKeyReleasedSignal().connect(&cam, &Camera::keyReleased);
	m_window.getEventsProcessedSignal().connect(&cam, &Camera::updateEvents);
	m_window.getKeyPressedSignal().connect([&](const uint32_t key)
	{
		if (key == SDLK_g)
		{
			cam.toggleMouseCapture();
			m_window.toggleMouseCapture();
		}
	});

	m_window.getSwapchainRebuiltSignal().connect(this, &Engine::recreateSwapchainResources);
}
