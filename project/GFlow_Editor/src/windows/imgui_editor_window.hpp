#pragma once
#include <string>

class ImGuiEditorWindow
{
public:
    virtual ~ImGuiEditorWindow() = default;
    explicit ImGuiEditorWindow(const std::string_view name) : m_name(name) {}

    virtual void draw() = 0;
    [[nodiscard]] std::string getName() const { return m_name; }

    bool open = true;

protected:
    std::string m_name;
};

#ifdef _DEBUG
#include <imgui.h>

class ImGuiTestWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiTestWindow(const std::string_view name) : ImGuiEditorWindow(name) {}

    void draw() override
    {
        ImGui::ShowDemoWindow(&open);
    }
};
#endif