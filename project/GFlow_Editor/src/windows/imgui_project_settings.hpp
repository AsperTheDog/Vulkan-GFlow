#pragma once
#include "windows/imgui_editor_window.hpp"

class ImGuiProjectSettingsWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiProjectSettingsWindow(const std::string_view& name, const bool defaultOpen = true);

    void draw() override;
};

