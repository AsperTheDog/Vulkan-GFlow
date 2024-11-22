#pragma once
#include "imgui_graph_window.hpp"
#include "ImNodeFlow.h"

class ImGuiExecutionWindow final : public ImGuiGraphWindow
{
public:
    explicit ImGuiExecutionWindow(const std::string_view& name, bool defaultOpen = true);
    void buildProject();

private:
    void rightClick(ImFlow::BaseNode* node) override;
};

