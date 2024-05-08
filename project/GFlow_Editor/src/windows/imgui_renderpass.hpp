#pragma once
#include "ImNodeFlow.h"
#include "windows/imgui_editor_window.hpp"

class ImGuiRenderPassWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiRenderPassWindow(const std::string_view& name);

    void draw() override;

    ImFlow::ImNodeFlow m_grid{"RenderPass"};
};

