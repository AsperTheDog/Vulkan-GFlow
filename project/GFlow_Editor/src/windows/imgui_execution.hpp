#pragma once
#include "ImNodeFlow.h"
#include "windows/imgui_editor_window.hpp"

class ImGuiExecutionWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiExecutionWindow(const std::string_view& name);

    void draw() override;

private:
    ImFlow::ImNodeFlow m_grid{"Execution"};
};

