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
    explicit ImGuiResourceEditorWindow(const std::string_view& name, bool defaultOpen = true);

    void resourceSelected(const std::string& resource);
    void resourceSelected(gflow::parser::Resource* resource);

    void draw() override;
    gflow::parser::Resource* getSelectedResource() const;

    void setInlinePadding(const float padding) { m_inlinePadding = padding; }

private:
    void drawFloat(const std::string& name, void* data) const;
    void drawInt(const std::string& name, void* data) const;
    void drawString(const std::string& name, void* data) const;
    void drawBool(const std::string& name, void* data) const;
    void drawVec2(const std::string& name, void* data) const;
    void drawVec3(const std::string& name, void* data) const;
    void drawVec4(const std::string& name, void* data) const;
    void drawResource(const std::string& stackedName, void* data, const std::vector<gflow::parser::Resource*>& parentPath) const;
    void drawSubresource(const std::string& name, std::string stackedName, gflow::parser::Resource::ExportData& data, const std::vector<gflow::parser::Resource*>& parentPath) const;
    void drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const;
    void drawBitmask(const std::string& name, void* data, const gflow::parser::EnumContext* context) const;


    gflow::parser::Resource* m_selectedResource = nullptr;

    mutable std::unordered_map<std::string, bool> m_nestedResourcesOpened;

private:
    float m_inlinePadding = 200.0f;
};

