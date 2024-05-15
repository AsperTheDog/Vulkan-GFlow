#pragma once
#include "resource.hpp"
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
    void drawElement(const gflow::parser::Resource::ExportData& element) const;

    void drawFloat(const std::string& name, void* data) const;
    void drawInt(const std::string& name, void* data) const;
    void drawString(const std::string& name, void* data) const;
    void drawBool(const std::string& name, void* data) const;
    void drawResource(const std::string& name, void* data, bool openTree = true) const;
    void drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const;

    template <typename T>
    void drawListHeader(const std::string& name, std::vector<T>* data) const;

    gflow::parser::Resource* m_selectedResource = nullptr;
};

