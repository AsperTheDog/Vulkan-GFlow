#pragma once
#include <unordered_map>

#include "imgui_editor_window.hpp"

namespace gflow::parser
{
    class Project;
}


class ImGuiResourcesWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourcesWindow(const std::string_view& name);

    void draw() override;
    void projectLoaded();

private:
    void pushFolder(std::vector<std::pair<std::string, bool>>& folderStack, const std::string& path, const std::string& folderName) const;
    void popFolder(std::vector<std::pair<std::string, bool>>& folderStack) const;

    [[nodiscard]] bool addTreeNode(const std::string& name, const std::string& path) const;
    bool addSelectable(const std::string& name, bool selected) const;

    std::string m_selectedResource;
};

