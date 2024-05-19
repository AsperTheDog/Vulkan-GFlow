#pragma once

#include "ImNodeFlow.h"
#include "project.hpp"
#include "sdl_window.hpp"
#include "windows/imgui_resources.hpp"

class Editor
{
public: // Internal Management
	static void init(const std::string& projectPath = "");
	static void run();

	static void cleanup();

private:
	static void createEnv();
	static void initImgui();
	static void connectSignals();

	static void renderFrame();
	static bool renderImgui();
	static void submitImgui();

	static void drawImgui();
	static void updateImguiWindows();
	static void recreateSwapchain(uint32_t width, uint32_t height);

    static void saveProject();

	inline static SDLWindow s_window{};
	inline static uint32_t s_environment = UINT32_MAX;
	inline static uint32_t s_imguiRenderPass = UINT32_MAX;
	inline static std::vector<uint32_t> s_imguiFrameBuffers{};

    inline static std::vector<ImGuiEditorWindow*> s_imguiWindows{};

    static ImGuiEditorWindow* getWindow(const std::string_view& name);

    
public: // Editor Management
    static void resourceSelected(const std::string& path);
    
    static void showCreateFolderModal(const std::string& path);
    static void showRenameFolderModal(const std::string& path);
    static void showDeleteFolderModal(const std::string& path);
    static void showCreateResourceModal(const std::string& path);
    static void showRenameResourceModal(const std::string& path);
    static void showDeleteResourceModal(const std::string& path);

    static void showResourcePickerModal(gflow::parser::Resource* parent, const std::string& variable);

private:
    static void createFolderModal();
    static void deleteFolderModal();
    static void renameFolderModal();
    static void createResourceModal();
    static void deleteResourceModal();
    static void renameResourceModal();

    static void newProjectModal();
    static void loadProjectModal();

    static void resourcePickerModal();

    inline static bool s_showCreateFolderModal;
    inline static bool s_showRenameFolderModal;
    inline static bool s_showDeleteFolderModal;
    inline static bool s_showCreateResourceModal;
    inline static bool s_showRenameResourceModal;
    inline static bool s_showDeleteResourceModal;
    inline static std::string s_modalBasePath;

    inline static bool s_showNewProjectModal;
    inline static bool s_showLoadProjectModal;

    inline static bool s_showResourcePickerModal;
    inline static gflow::parser::Resource* s_resourcePickerParent;
    inline static std::string s_resourcePickerElement;

    inline static ImGuiResourcesWindow s_getResourceRefWindow{"Resource Picker"};

    inline static Signal<> s_projectLoadedSignal;

    inline static Signal<const std::string&> s_resourceSelectedSignal;
};

