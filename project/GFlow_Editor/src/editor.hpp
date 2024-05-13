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

	inline static SDLWindow s_window{};
	inline static uint32_t s_environment = UINT32_MAX;
	inline static uint32_t s_imguiRenderPass = UINT32_MAX;
	inline static std::vector<uint32_t> s_imguiFrameBuffers{};

    inline static std::vector<ImGuiEditorWindow*> s_imguiWindows{};

    static ImGuiEditorWindow* getWindow(const std::string_view& name);

    
public: // Editor Management
    [[nodiscard]] static gflow::parser::Project& getProject();
    static void resourceSelected(const std::string& path);

    [[nodiscard]] static bool hasProject();
    
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

    inline static std::optional<gflow::parser::Project> m_project;

    inline static bool s_showCreateFolderModal;
    inline static bool s_showRenameFolderModal;
    inline static bool s_showDeleteFolderModal;
    inline static bool s_showCreateResourceModal;
    inline static bool s_showRenameResourceModal;
    inline static bool s_showDeleteResourceModal;
    inline static std::string s_modalBasePath;

    inline static Signal<> s_projectLoadedSignal;

    inline static Signal<const std::string&> s_folderCreatedSignal;
    inline static Signal<const std::string&> s_folderDeletedSignal;
    inline static Signal<const std::string&, const std::string&> s_folderRenamedSignal;
    inline static Signal<const std::string&> s_resourceCreatedSignal;
    inline static Signal<const std::string&> s_resourceDeletedSignal;
    inline static Signal<const std::string&, const std::string&> s_resourceRenamedSignal;

    inline static Signal<std::string_view> s_resourceSelectedSignal;
};

