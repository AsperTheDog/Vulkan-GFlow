#pragma once
#include "sdl_window.hpp"


class Editor
{
public:
	static void init();
	static void run();

	static void cleanup();

private:
	static void createEnv();
	static void initImgui();

	static void renderFrame();
	static bool renderImgui();
	static void submitImgui();

	static void drawImgui();
	static void updateImguiWindows();

	inline static SDLWindow s_window{};
	inline static uint32_t s_environment = UINT32_MAX;
	inline static uint32_t s_imguiRenderPass = UINT32_MAX;
	inline static std::vector<uint32_t> s_imguiFrameBuffers{};
};

