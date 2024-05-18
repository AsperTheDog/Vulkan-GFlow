#include "imgui_resources.hpp"

#include <filesystem>

#include "editor.hpp"
#include "resource_manager.hpp"
#include "string_helper.hpp"



ImGuiResourcesWindow::ImGuiResourcesWindow(const std::string_view& name)
    : ImGuiEditorWindow(name)
{
}

void ImGuiResourcesWindow::pushFolder(std::vector<std::pair<std::string, bool>>& folderStack, const std::string& path, const std::string& folderName) const
{
    folderStack.emplace_back(path, addTreeNode(folderName, path));
}

void ImGuiResourcesWindow::popFolder(std::vector<std::pair<std::string, bool>>& folderStack) const
{
    if (folderStack.back().second)
    {
        ImGui::TreePop();
    }
    folderStack.pop_back();
}

bool ImGuiResourcesWindow::addTreeNode(const std::string& name, const std::string& path) const
{
    const bool opened = ImGui::TreeNode(name.c_str());
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text(("Make changes to " + name).c_str());
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.0f);
        ImGui::BeginDisabled(path.empty());
        if (ImGui::MenuItem("Delete"))
        {
            Editor::showDeleteFolderModal(path);
        }
        if (ImGui::MenuItem("Rename"))
        {
            Editor::showRenameFolderModal(path);
        }
        ImGui::EndDisabled();
        ImGui::Separator();
        if (ImGui::MenuItem("Create folder"))
        {
            Editor::showCreateFolderModal(path);
        }
        if (ImGui::MenuItem("Create resource"))
        {
            Editor::showCreateResourceModal(path);
        }
        ImGui::EndPopup();
    }
    return opened;
}

bool ImGuiResourcesWindow::addSelectable(const std::string& name, const bool selected) const
{
    const bool ret = ImGui::Selectable(name.c_str(), selected);
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text(("Make changes to " + name).c_str());
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.0f);
        if (ImGui::MenuItem("Delete"))
        {

        }
        if (ImGui::MenuItem("Rename"))
        {

        }
        ImGui::EndPopup();
    }
    return ret;
}

void ImGuiResourcesWindow::draw()
{
    if (!gflow::parser::ResourceManager::hasProject()) return;

    std::vector<std::pair<std::string, bool>> folderStack;

    ImGui::Begin(m_name.c_str(), &open);

    if (ImGui::Button("Refresh")) projectLoaded();

    ImGui::Separator();

    if (!addTreeNode("root", ""))
    {
        ImGui::End();
        return;
    }

    const std::vector<std::string> orderedPaths = gflow::parser::ResourceManager::getTree().getOrderedPaths();
    for (const std::string& path : orderedPaths)
    {
        std::string name = gflow::string::getPathFilename(path);
        std::string parentDir = gflow::string::getPathDirectory(path);
        if (!folderStack.empty() && path == folderStack.back().first)
        {
            popFolder(folderStack);
            continue;
        }
        if (!parentDir.empty() && !folderStack.back().second) continue;

        if (path.back() == '/')
        {
            pushFolder(folderStack, path, name);
            continue;
        }
        if (gflow::parser::ResourceManager::hasResource(path) && addSelectable(name, path == m_selectedResource) && path != m_selectedResource)
        {
            m_selectedResource = path;
            Editor::resourceSelected(path);
        }
    }

    ImGui::TreePop();
    ImGui::End();
}

void ImGuiResourcesWindow::projectLoaded()
{
    Editor::resourceSelected("");
}
