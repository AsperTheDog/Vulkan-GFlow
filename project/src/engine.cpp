#include "engine.hpp"

#include <stdexcept>

#include <imgui.h>
#include <glm/vec3.hpp>

#include "logger.hpp"
#include "backends/imgui_impl_vulkan.h"
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

	//TODO: Create other objects (render pass, pipeline, etc.)

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

		Engine::drawImgui();

		ImGui::Render();
		ImDrawData* imguiDrawData = ImGui::GetDrawData();

		if (imguiDrawData->DisplaySize.x <= 0.0f || imguiDrawData->DisplaySize.y <= 0.0f)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			continue;
		}

		//TODO: Record

		//TODO: submit

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		//TODO: present

		frameCounter++;
	}
}

void Engine::createDevice()
{
	const VulkanGPU gpu = chooseCorrectGPU();
	const GPUQueueStructure queueStructure = gpu.getQueueFamilies();
	QueueFamilySelector queueFamilySelector(queueStructure);

	// Select Queue Families and assign queues
	//TODO: Select queue families
	const QueueFamily transferQueueFamily = queueStructure.findQueueFamily(VK_QUEUE_TRANSFER_BIT);

	QueueFamilySelector selector{queueStructure};
	//TODO: Configure queues
	m_transferQueuePos = selector.addQueue(transferQueueFamily, 1.0);

	m_deviceID = VulkanContext::createDevice(gpu, selector, {VK_KHR_SWAPCHAIN_EXTENSION_NAME}, {});
	VulkanContext::getDevice(m_deviceID).configureOneTimeQueue(m_transferQueuePos);
}

void Engine::createSwapchain()
{
	m_window.createSwapchain(m_deviceID, {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});

	m_framebuffers.resize(m_window.getImageCount());
	for (uint32_t i = 0; i < m_window.getImageCount(); i++)
	{
		//TODO: Create swapchain framebuffers
	}
}

void Engine::createSyncObjects()
{
	VulkanDevice& device = VulkanContext::getDevice(m_deviceID);
	m_imageAvailableSemaphoreID = device.createSemaphore();
	m_renderFinishedSemaphoreID = device.createSemaphore();
	m_inFlightFenceID = device.createFence(true);
}

void Engine::recreateSwapchainResources(VkExtent2D extent)
{
	Logger::pushContext("Swapchain resources rebuild");
	for (uint32_t i = 0; i < m_window.getImageCount(); i++)
	{
		VulkanContext::getDevice(m_deviceID).freeFramebuffer(m_framebuffers[i]);
		//TODO: Create swapchain framebuffers
	}
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
    //TODO: init_info.QueueFamily = m_graphicsQueuePos.familyIndex;
    //TODO: init_info.Queue = *device.getQueue(m_graphicsQueuePos);
    init_info.DescriptorPool = *device.getDescriptorPool(imguiPoolID);
    //TODO: init_info.RenderPass = *device.getRenderPass(m_renderPassID);
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

void Engine::drawImgui() const
{
	
}
