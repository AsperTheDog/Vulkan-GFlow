#include "imgui_resource_editor.hpp"

#include "imgui.h"

ImGuiResourceEditorWindow::ImGuiResourceEditorWindow(const std::string_view& name) : ImGuiEditorWindow(name)
{
}

void ImGuiResourceEditorWindow::draw()
{
    ImGui::Begin(name.c_str(), &open);

    ImGui::End();
}
