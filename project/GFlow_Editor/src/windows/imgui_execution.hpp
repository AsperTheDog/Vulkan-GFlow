#pragma once
#include "ImNodeFlow.h"
#include "windows/imgui_editor_window.hpp"

class ImGuiExecutionWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiExecutionWindow(const std::string_view& name, const bool defaultOpen = true);

    void draw() override;

private:
    void rightClick(const ImFlow::BaseNode* node) const;

    ImFlow::ImNodeFlow m_grid{"Execution"};
};

