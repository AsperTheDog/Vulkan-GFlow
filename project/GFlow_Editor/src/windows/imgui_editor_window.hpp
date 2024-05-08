#pragma once
#include <string>

class ImGuiEditorWindow
{
public:
    explicit ImGuiEditorWindow(const std::string_view name) : name(name) {}

    virtual void draw() = 0;
    [[nodiscard]] std::string getName() const { return name; }

    bool open = true;

protected:
    std::string name;
};

#ifdef _DEBUG
#include <imgui.h>

class ImGuiTestWindow : public ImGuiEditorWindow
{
public:
    explicit ImGuiTestWindow(const std::string_view name) : ImGuiEditorWindow(name) {}

    void draw() override
    {
        ImGui::ShowDemoWindow(&open);
    }
};
#endif