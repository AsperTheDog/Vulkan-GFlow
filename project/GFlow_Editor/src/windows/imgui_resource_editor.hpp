#pragma once
#include "resource.hpp"
#include "utils/signal.hpp"
#include "windows/imgui_editor_window.hpp"

namespace gflow::parser { class Resource; }

class ImGuiResourceEditorWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourceEditorWindow(const std::string& name, bool defaultOpen = true);

    void resourceSelected(const std::string& resource);
    void resourceSelected(gflow::parser::Resource* resource);

    void draw() override;
    gflow::parser::Resource* getSelectedResource() const;

    void setInlinePadding(const float padding) { m_inlinePadding = padding; }
    
    void updateChangedVar(const gflow::parser::Resource* resource, const std::string& name, bool commit);

    [[nodiscard]] Signal<const gflow::parser::ResourceElemPath&>& getVariableChangedSignal() { return m_variableChangedSignal; }

private:
    bool drawFloat(const std::string& name, void* data) const;
    bool drawInt(const std::string& name, void* data) const;
    bool drawBigInt(const std::string& name, void* data) const;
    bool drawString(const std::string& name, void* data, bool isShort = false) const;
    bool drawBool(const std::string& name, void* data) const;
    bool drawVec2(const std::string& name, void* data) const;
    bool drawVec3(const std::string& name, void* data) const;
    bool drawVec4(const std::string& name, void* data) const;
    bool drawMat3(const std::string& name, void* data) const;
    bool drawMat4(const std::string& name, void* data) const;
    bool drawColor(const std::string& name, void* data) const;
    bool drawUColor(const std::string& name, void* data) const;
    void drawResource(const std::string& stackedName, void* data, const std::vector<gflow::parser::Resource*>& parentPath);
    void drawSubresource(const std::string& name, std::string stackedName, gflow::parser::Resource::ExportData& data, const std::vector<gflow::parser::Resource*>& parentPath);
    bool drawEnum(const std::string& name, void* data, const gflow::parser::EnumContext* context) const;
    bool drawBitmask(const std::string& name, void* data, const gflow::parser::EnumContext* context) const;
    bool drawFile(const std::string& name, void* data) const;


    gflow::parser::Resource* m_selectedResource = nullptr;

    mutable std::unordered_map<std::string, bool> m_nestedResourcesOpened;

    std::vector<gflow::parser::ResourceElemPath> m_variablesFlaggedToChange;

    Signal<const gflow::parser::ResourceElemPath&> m_variableChangedSignal;

private:
    float m_inlinePadding = 200.0f;
};

