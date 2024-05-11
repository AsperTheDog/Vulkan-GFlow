#pragma once
#include "ImNodeFlow.h"
#include "windows/imgui_editor_window.hpp"

class ImGuiPipelineWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiPipelineWindow(const std::string_view& name);

    void draw() override;

    ImFlow::ImNodeFlow m_grid{"RenderPass"};
};

