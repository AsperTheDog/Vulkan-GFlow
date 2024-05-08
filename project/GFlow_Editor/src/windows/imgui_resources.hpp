#pragma once
#include <imgui_internal.h>

#include "imgui_editor_window.hpp"

class ImGuiResourcesWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourcesWindow(const std::string_view& name);

    void draw() override;
};

