#pragma once
#include "ImNodeFlow.h"
#include "windows/imgui_editor_window.hpp"

namespace gflow::parser
{
    class Resource;
}

class ImGuiRenderPassWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiRenderPassWindow(const std::string_view& name);
    void resourceSelected(const std::string& resource);

    void draw() override;

    ImFlow::ImNodeFlow m_grid{"RenderPass"};

private:
    void rightClick(ImFlow::BaseNode* node);

    gflow::parser::Resource* m_selectedPass = nullptr;
};

