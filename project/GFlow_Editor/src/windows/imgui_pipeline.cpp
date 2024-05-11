#include "imgui_pipeline.hpp"

#include "imgui.h"

ImGuiPipelineWindow::ImGuiPipelineWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiPipelineWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    m_grid.update();
    ImGui::End();
}
