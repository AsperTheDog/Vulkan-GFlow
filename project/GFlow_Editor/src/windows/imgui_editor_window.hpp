#pragma once
#include <string>

#define IMGUI_NAME(name) (name + std::string("##") + m_name).c_str()

class ImGuiEditorWindow
{
public:
    virtual ~ImGuiEditorWindow() = default;
    explicit ImGuiEditorWindow(const std::string_view name, const bool defaultOpen) : open(defaultOpen), m_name(name) {}

    virtual void draw() = 0;
    [[nodiscard]] std::string getName() const { return m_name; }

    virtual void save() {}

    bool open;

protected:
    std::string m_name;
};

#ifdef _DEBUG
#include <imgui.h>

class ImGuiTestWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiTestWindow(const std::string_view name, const bool defaultOpen = true) : ImGuiEditorWindow(name, defaultOpen) {}

    void draw() override
    {
        ImGui::ShowDemoWindow(&open);
    }
};
#endif