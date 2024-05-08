#pragma once
#include "windows/imgui_editor_window.hpp"

class ImGuiResourceEditorWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourceEditorWindow(const std::string_view& name);

    void draw() override;
};

