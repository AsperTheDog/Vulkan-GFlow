#pragma once
#include <unordered_map>

#include "imgui_editor_window.hpp"
#include "utils/signal.hpp"

namespace gflow::parser
{
    class Project;
}


class ImGuiResourcesWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourcesWindow(const std::string& name, const bool defaultOpen = true);

    void setFilter(const std::string& filter) { m_typeFilter = filter; }

    void draw() override;
    void drawContent();
    void projectLoaded();

    [[nodiscard]] std::string getSelectedResource() const { return m_selectedResource; }
    [[nodiscard]] Signal<const std::string&>& getResourceSelectedSignal() { return m_resourceSelectedSignal; }

private:
    void pushFolder(std::vector<std::pair<std::string, bool>>& folderStack, const std::string& path, const std::string& folderName) const;
    void popFolder(std::vector<std::pair<std::string, bool>>& folderStack) const;

    [[nodiscard]] bool addTreeNode(const std::string& name, const std::string& path) const;
    [[nodiscard]] bool addSelectable(const std::string& name, bool selected) const;

    std::string m_typeFilter;
    std::string m_selectedResource;

    Signal<const std::string&> m_resourceSelectedSignal;
};

