#pragma once
#include <optional>

#include "ImNodeFlow.h"
#include "project.hpp"
#include "sdl_window.hpp"

class ImGuiEditorWindow;

class Editor
{
public: // Internal Management
	static void init();
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

private:
    static void createFolderModal();
    static void deleteFolderModal();
    static void renameFolderModal();
    static void createResourceModal();
    static void deleteResourceModal();
    static void renameResourceModal();


    static void newProjectModal();
    static void loadProjectModal();

    inline static bool s_showCreateFolderModal;
    inline static bool s_showRenameFolderModal;
    inline static bool s_showDeleteFolderModal;
    inline static bool s_showCreateResourceModal;
    inline static bool s_showRenameResourceModal;
    inline static bool s_showDeleteResourceModal;
    inline static std::string s_modalBasePath;

    inline static bool s_showNewProjectModal;
    inline static bool s_showLoadProjectModal;

    inline static Signal<> s_projectLoadedSignal;

    inline static Signal<const std::string&> s_resourceSelectedSignal;
};

