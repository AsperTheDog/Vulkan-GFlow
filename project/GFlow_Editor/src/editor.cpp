#include "editor.hpp"

#include <filesystem>

#include "context.hpp"
#include "imgui.h"
#include "resource_manager.hpp"
#include "string_helper.hpp"
#include "vulkan_context.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "metaresources/renderpass.hpp"
#include "utils/logger.hpp"
#include "windows/imgui_execution.hpp"
#include "windows/imgui_project_settings.hpp"
#include "windows/imgui_renderpass.hpp"

#include "windows/imgui_resources.hpp"
#include "windows/imgui_resource_editor.hpp"

void Editor::init(const std::string& projectPath)
{
    Logger::setLevels(Logger::ALL);
    Logger::setRootContext("Window init");

    s_window = SDLWindow{ "GFlow", 1280, 720 };

    Logger::setRootContext("Vulkan init");
    gflow::Context::initVulkan(s_window.getRequiredVulkanExtensions());
    s_window.createSurface(gflow::Context::getVulkanInstance());

    createEnv();
    initImgui();
    connectSignals();
    createWindows();

    if (!projectPath.empty())
    {
        gflow::parser::ResourceManager::loadProject(projectPath);
        s_projectLoadedSignal.emit();
    }
}

void Editor::run()
{
    while (!s_window.shouldClose())
    {
        s_window.pollEvents();
        renderFrame();
        updateImguiWindows();
    }
}

void Editor::renderFrame()
{
    gflow::Environment& env = gflow::Context::getEnvironment(s_environment);

    env.beginRecording({ s_window.getSurface() });

    // Build and render projects here

    if (!renderImgui()) return;
    env.setRecordingBarrier();
    submitImgui();

    env.endRecording();
    env.present(s_window.getSurface());
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

    env.configurePresentTarget(s_window.getSurface(), s_window.getSize().toExtent2D(), { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });
}

void Editor::initImgui()
{
    Logger::pushContext("Init Imgui");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

    const uint32_t imguiPoolID = device.createDescriptorPool(pool_sizes, 1000 * static_cast<uint32_t>(pool_sizes.size()), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
    const VulkanSwapchain& swapchain = device.getSwapchain(env.man_getSwapchain(s_window.getSurface()));

    {
        Logger::pushContext("Create Imgui Renderpass");
        VulkanRenderPassBuilder builder{};
        const VkAttachmentDescription colorAttachment = VulkanRenderPassBuilder::createAttachment(swapchain.getFormat().format,
            VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        builder.addAttachment(colorAttachment);
        builder.addSubpass(VK_PIPELINE_BIND_POINT_GRAPHICS, { {COLOR, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} }, 0);
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
    const QueueSelection queuePos = env.man_getQueuePos(QueueFamilyTypeBits::GRAPHICS);

    s_window.initImgui();
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = VulkanContext::getHandle();
    init_info.PhysicalDevice = *device.getGPU();
    init_info.Device = *device;
    init_info.QueueFamily = queuePos.familyIndex;
    init_info.Queue = *device.getQueue(queuePos);
    init_info.DescriptorPool = *device.getDescriptorPool(imguiPoolID);
    init_info.RenderPass = *renderPass;
    init_info.Subpass = 0;
    init_info.MinImageCount = swapchain.getMinImageCount();
    init_info.ImageCount = swapchain.getImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    ImGui_ImplVulkan_Init(&init_info);
    Logger::popContext();

    const VkExtent3D extent = { swapchain.getExtent().width, swapchain.getExtent().height, 1 };
    for (uint32_t i = 0; i < swapchain.getImageCount(); ++i)
        s_imguiFrameBuffers.push_back(device.createFramebuffer(extent, renderPass, { swapchain.getImageView(i) }));
}

void Editor::connectSignals()
{
    s_window.getResizeSignal().connect(Editor::recreateSwapchain);
    s_window.getSaveInputSignal().connect(Editor::saveProject);
}

void Editor::createWindows()
{
    s_imguiWindows.push_back(new ImGuiResourcesWindow("Resources"));
    s_imguiWindows.push_back(new ImGuiResourceEditorWindow("Resource Editor"));
    s_imguiWindows.push_back(new ImGuiExecutionWindow("Execution"));
    s_imguiWindows.push_back(new ImGuiRenderPassWindow("RenderPass"));
    s_imguiWindows.push_back(new ImGuiProjectSettingsWindow("Project Settings", false));
#ifdef _DEBUG
    s_imguiWindows.push_back(new ImGuiTestWindow("Test", false));
#endif

    {
        // Setup resources window
        ImGuiResourcesWindow* resourcesWindow = dynamic_cast<ImGuiResourcesWindow*>(getWindow("Resources"));
        s_projectLoadedSignal.connect(resourcesWindow, &ImGuiResourcesWindow::projectLoaded);
        resourcesWindow->getResourceSelectedSignal().connect(&Editor::resourceSelected);
    }
    {
        // Setup resource editor window
        ImGuiResourceEditorWindow* resourceEditorWindow = dynamic_cast<ImGuiResourceEditorWindow*>(getWindow("Resource Editor"));
        s_resourceSelectedSignal.connect(resourceEditorWindow, &ImGuiResourceEditorWindow::resourceSelected);
    }
    {
        // Setup renderpass window
        ImGuiRenderPassWindow* renderPassWindow = dynamic_cast<ImGuiRenderPassWindow*>(getWindow("RenderPass"));
        s_resourceSelectedSignal.connect(renderPassWindow, &ImGuiRenderPassWindow::resourceSelected);
    }
    {
        // Setup execution window
        ImGuiExecutionWindow* executionWindow = dynamic_cast<ImGuiExecutionWindow*>(getWindow("Execution"));
        s_projectLoadedSignal.connect(executionWindow, &ImGuiExecutionWindow::onProjectLoaded);
    }
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

    const std::vector<VkClearValue> clearValues{ {0.0f, 0.0f, 0.0f, 1.0f} };

    const uint32_t frameBuffer = s_imguiFrameBuffers[env.man_getSwapchainImage(s_window.getSurface())];

    commandBuffer.cmdBeginRenderPass(s_imguiRenderPass, frameBuffer, s_window.getSize().toExtent2D(), clearValues);
    ImGui_ImplVulkan_RenderDrawData(imguiDrawData, *commandBuffer);
    commandBuffer.cmdEndRenderPass();
}

void Editor::drawImgui()
{
    // Main menu bar
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Project"))
    {
        if (ImGui::MenuItem("New project"))
        {
            s_showNewProjectModal = true;
        }
        if (ImGui::MenuItem("Load project"))
        {
            s_showLoadProjectModal = true;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Save project"))
        {
            saveProject();
        }
        ImGui::Separator();
        ImGui::MenuItem("Project Settings", "", &getWindow("Project Settings")->open);

        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Windows"))
    {
        for (ImGuiEditorWindow* window : s_imguiWindows)
        {
            ImGui::MenuItem(window->getName().c_str(), "", &window->open);
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();

    // General window execution
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    for (ImGuiEditorWindow* window : s_imguiWindows)
    {
        if (window->open)
        {
            window->draw();
        }
    }

    createFolderModal();
    deleteFolderModal();
    renameFolderModal();
    createResourceModal();
    deleteResourceModal();
    renameResourceModal();

    newProjectModal();
    loadProjectModal();

    resourcePickerModal();
}

void Editor::updateImguiWindows()
{
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}

void Editor::recreateSwapchain(const uint32_t width, const uint32_t height)
{
    Logger::pushContext("Recreate Swapchain");
    gflow::Environment& env = gflow::Context::getEnvironment(s_environment);
    VulkanDevice& device = VulkanContext::getDevice(env.man_getDevice());

    env.reconfigurePresentTarget(s_window.getSurface(), { width, height });

    const VulkanRenderPass& renderPass = device.getRenderPass(s_imguiRenderPass);
    const VulkanSwapchain& swapchain = device.getSwapchain(env.man_getSwapchain(s_window.getSurface()));

    const VkExtent3D extent = { width, height, 1 };
    for (uint32_t i = 0; i < swapchain.getImageCount(); ++i)
    {
        device.freeFramebuffer(s_imguiFrameBuffers[i]);
        s_imguiFrameBuffers[i] = device.createFramebuffer(extent, renderPass, { swapchain.getImageView(i) });
    }
    Logger::popContext();
}

void Editor::saveProject()
{
    for (ImGuiEditorWindow* window : s_imguiWindows)
        window->save();
    
    gflow::parser::ResourceManager::saveAll();
}

ImGuiEditorWindow* Editor::getWindow(const std::string_view& name)
{
    for (ImGuiEditorWindow* window : s_imguiWindows)
    {
        if (window->getName() == name)
        {
            return window;
        }
    }
    return nullptr;
}

void Editor::resourceSelected(const std::string& path)
{
    s_resourceSelectedSignal.emit(path);
    s_selectedResource = path;
}

gflow::parser::Resource* Editor::getSelectedResource()
{
    return gflow::parser::ResourceManager::getResource(s_selectedResource);
}

gflow::parser::Project* Editor::getCurrentProject()
{
    return dynamic_cast<gflow::parser::Project*>(gflow::parser::ResourceManager::getProject());
}

void Editor::showCreateFolderModal(const std::string& path)
{
    s_showCreateFolderModal = true;
    s_modalBasePath = path;
}

void Editor::showRenameFolderModal(const std::string& path)
{
    s_showRenameFolderModal = true;
    s_modalBasePath = path;
}

void Editor::showDeleteFolderModal(const std::string& path)
{
    s_showDeleteFolderModal = true;
    s_modalBasePath = path;
}

void Editor::showCreateResourceModal(const std::string& path)
{
    s_showCreateResourceModal = true;
    s_modalBasePath = path;
}

void Editor::showRenameResourceModal(const std::string& path)
{
    s_showRenameResourceModal = true;
    s_modalBasePath = path;
}

void Editor::showDeleteResourceModal(const std::string& path)
{
    s_showDeleteResourceModal = true;
    s_modalBasePath = path;
}

void Editor::showResourcePickerModal(ImGuiResourceEditorWindow* caller, gflow::parser::Resource* parent, const std::string& variable, const std::string& filter)
{
    s_resourcePickerCaller = caller;
    s_resourcePickerParent = parent;
    s_resourcePickerElement = variable;
    s_getResourceRefWindow.setFilter(filter);
    s_showResourcePickerModal = true;
}

void Editor::resourcePickerModal()
{
    if (s_showResourcePickerModal)
    {
        ImGui::OpenPopup("Resource Picker");
        s_showResourcePickerModal = false;
    }

    if (ImGui::BeginPopupModal("Resource Picker", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Select a resource to edit");
        ImGui::BeginChild("Picker filetree", ImVec2(400, 500), ImGuiChildFlags_Border);
        s_getResourceRefWindow.drawContent();
        ImGui::EndChild();
        ImGui::BeginDisabled(s_getResourceRefWindow.getSelectedResource().empty());
        if (ImGui::Button("Select##Picker"))
        {
            s_resourcePickerParent->set(s_resourcePickerElement, s_getResourceRefWindow.getSelectedResource(), {});
            s_resourcePickerCaller->updateChangedVar(s_resourcePickerParent, s_resourcePickerElement, true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel##Picker"))
        {
            s_resourcePickerCaller->updateChangedVar(s_resourcePickerParent, s_resourcePickerElement, false);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::createFolderModal()
{
    if (s_showCreateFolderModal)
    {
        ImGui::OpenPopup("New Folder");
        s_showCreateFolderModal = false;
    }

    if (ImGui::BeginPopupModal("New Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char folderName[128] = "";
        ImGui::InputText("Folder Name", folderName, IM_ARRAYSIZE(folderName));
        ImGui::BeginDisabled(strcmp(folderName, "") == 0);
        if (ImGui::Button("Create"))
        {
            const std::string folderPath = gflow::parser::ResourceManager::makePathAbsolute(folderName);
            if (std::filesystem::create_directory(folderPath))
            {
                Logger::print("Folder created at: " + folderPath, Logger::INFO);
                gflow::parser::ResourceManager::getTree().addPath(s_modalBasePath + folderName + "/");
            }
            else
            {
                Logger::print("Failed to create folder at: " + folderPath, Logger::ERR);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::deleteFolderModal()
{
    if (s_showDeleteFolderModal)
    {
        ImGui::OpenPopup("Delete Folder");
        s_showDeleteFolderModal = false;
    }

    if (ImGui::BeginPopupModal("Delete Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Folder Name: %s", gflow::string::getPathFilename(s_modalBasePath).c_str());
        ImGui::Text("Are you sure you want to delete this folder?");
        if (ImGui::Button("Confirm"))
        {
            const std::string baseDir = gflow::parser::ResourceManager::makePathAbsolute(s_modalBasePath);
            Logger::print("Folder deleted at: " + baseDir, Logger::INFO);
            gflow::parser::ResourceManager::deleteDirectory(s_modalBasePath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::renameFolderModal()
{
    if (s_showRenameFolderModal)
    {
        ImGui::OpenPopup("Rename Folder");
        s_showRenameFolderModal = false;
    }

    if (ImGui::BeginPopupModal("Rename Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        std::string renameFolderName = gflow::string::getPathFilename(s_modalBasePath);

        ImGui::BeginDisabled(true);
        ImGui::InputText("Old folder Name", renameFolderName.data(), renameFolderName.size());
        ImGui::EndDisabled();
        static char folderName[128] = "";
        ImGui::InputText("New folder Name", folderName, IM_ARRAYSIZE(folderName));
        ImGui::BeginDisabled(strcmp(folderName, "") == 0 || strcmp(folderName, renameFolderName.c_str()) == 0);
        if (ImGui::Button("Rename"))
        {
            const std::string baseDir = gflow::parser::ResourceManager::makePathAbsolute(s_modalBasePath);
            const std::string folderPath = gflow::string::replacePathFilename(baseDir, folderName);
            std::filesystem::rename(baseDir, folderPath);
            Logger::print("Folder renamed from: " + baseDir + " to: " + folderPath, Logger::INFO);
            gflow::parser::ResourceManager::getTree().renamePath(gflow::string::replacePathFilename(s_modalBasePath, renameFolderName), folderName);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::createResourceModal()
{
    if (s_showCreateResourceModal)
    {
        ImGui::OpenPopup("Create Resource");
        s_showCreateResourceModal = false;
    }

    if (ImGui::BeginPopupModal("Create Resource", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char resourceName[128] = "";
        ImGui::InputText("Resource Name", resourceName, IM_ARRAYSIZE(resourceName));
        std::vector<std::string> types = gflow::parser::ResourceManager::getResourceTypes();
        std::ranges::sort(types);

        static uint32_t currentSelection = 0;
        if (!types.empty() && ImGui::BeginCombo("Resource type", types[currentSelection].c_str(), 0))
        {
            for (uint32_t n = 0; n < types.size(); n++)
            {
                const bool isSelected = currentSelection == n;
                if (ImGui::Selectable(types[n].c_str(), isSelected))
                    currentSelection = n;

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::BeginDisabled(!types.empty() && strcmp(resourceName, "") == 0 || resourceName[0] == '_');
        if (ImGui::Button("Create"))
        {
            gflow::parser::ResourceManager::createResource(types[currentSelection], s_modalBasePath + resourceName);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::deleteResourceModal()
{
    if (s_showDeleteResourceModal)
    {
        ImGui::OpenPopup("Delete Resource");
        s_showDeleteResourceModal = false;
    }

    if (ImGui::BeginPopupModal("Delete Resource", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Resource Name: %s", gflow::string::getPathFilename(s_modalBasePath).c_str());
        ImGui::Text("Are you sure you want to delete this resource?");
        if (ImGui::Button("Confirm"))
        {
            const std::string absPath = gflow::parser::ResourceManager::makePathAbsolute(s_modalBasePath);
            std::filesystem::remove_all(absPath);
            Logger::print("Resource deleted at: " + absPath, Logger::INFO);
            gflow::parser::ResourceManager::deleteResource(s_modalBasePath);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::renameResourceModal()
{
}

void Editor::newProjectModal()
{
    if (s_showNewProjectModal)
    {
        ImGui::OpenPopup("New Project");
        s_showNewProjectModal = false;
    }

    if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char projectName[128] = "";
        ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
        static char projectPath[128] = "";
        ImGui::InputText("Project Path", projectPath, IM_ARRAYSIZE(projectPath));

        ImGui::BeginDisabled(strcmp(projectName, "") == 0 || strcmp(projectPath, "") == 0 || !std::filesystem::exists(projectPath) || !std::filesystem::is_empty(projectPath));
        if (ImGui::Button("Create"))
        {
            gflow::parser::ResourceManager::resetWorkingDir(projectPath);
            gflow::parser::ResourceManager::createProject(projectName);
            s_projectLoadedSignal.emit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::loadProjectModal()
{
    if (s_showLoadProjectModal)
    {
        ImGui::OpenPopup("Load Project");
        s_showLoadProjectModal = false;
    }

    if (ImGui::BeginPopupModal("Load Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char projectPath[128] = "";
        ImGui::InputText("Project Path", projectPath, IM_ARRAYSIZE(projectPath));

        ImGui::BeginDisabled(strcmp(projectPath, "") == 0 || !std::filesystem::exists(projectPath));
        if (ImGui::Button("Load"))
        {
            gflow::parser::ResourceManager::loadProject(projectPath);
            s_projectLoadedSignal.emit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
