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

    void resourceSelected(const std::string& resource);

    void draw() override;

private:
    void drawFloat(const std::string& name, void* data) const;
    void drawInt(const std::string& name, void* data) const;
    void drawString(const std::string& name, void* data) const;
    void drawBool(const std::string& name, void* data) const;
    void drawResource(const std::string& stackedName, void* data) const;
    void drawSubresource(const std::string& name, std::string stackedName, const gflow::parser::Resource::ExportData& data, gflow::parser::Resource* parent) const;
    void drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const;


    gflow::parser::Resource* m_selectedResource = nullptr;

    mutable std::unordered_map<std::string, bool> m_nestedResourcesOpened;
};

