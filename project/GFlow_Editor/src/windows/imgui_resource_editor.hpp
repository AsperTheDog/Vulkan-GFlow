#pragma once
#include "windows/imgui_editor_window.hpp"

namespace gflow::parser
{
    class Resource;
}

class ImGuiResourceEditorWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourceEditorWindow(const std::string_view& name);

    void resourceSelected(std::string_view resource);

    void draw() override;

private:
    gflow::parser::Resource* m_selectedResource = nullptr;
};

