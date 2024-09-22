#include "imgui_resources.hpp"

#include <filesystem>

#include "editor.hpp"
#include "resource_manager.hpp"
#include "string_helper.hpp"



ImGuiResourcesWindow::ImGuiResourcesWindow(const std::string_view& name, const bool defaultOpen) : ImGuiEditorWindow(name, defaultOpen)
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
        if (ImGui::MenuItem(IMGUI_NAME("Delete")))
        {
            Editor::showDeleteFolderModal(path);
        }
        if (ImGui::MenuItem(IMGUI_NAME("Rename")))
        {
            Editor::showRenameFolderModal(path);
        }
        ImGui::EndDisabled();
        ImGui::Separator();
        if (ImGui::MenuItem(IMGUI_NAME("Create folder")))
        {
            Editor::showCreateFolderModal(path);
        }
        if (ImGui::MenuItem(IMGUI_NAME("Create resource")))
        {
            Editor::showCreateResourceModal(path);
        }
        ImGui::EndPopup();
    }
    return opened;
}

bool ImGuiResourcesWindow::addSelectable(const std::string& name, const bool selected) const
{
    const bool ret = ImGui::Selectable(IMGUI_NAME(name), selected);
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::Text(("Make changes to " + name).c_str());
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.0f);
        if (ImGui::MenuItem(IMGUI_NAME("Delete")))
        {

        }
        if (ImGui::MenuItem(IMGUI_NAME("Rename")))
        {

        }
        ImGui::EndPopup();
    }
    return ret;
}

void ImGuiResourcesWindow::draw()
{
    ImGui::Begin(m_name.c_str(), &open);
    drawContent();
    ImGui::End();
}

void ImGuiResourcesWindow::drawContent()
{
    if (!gflow::parser::ResourceManager::hasProject()) return;

    std::vector<std::pair<std::string, bool>> folderStack;

    if (ImGui::Button(IMGUI_NAME("Refresh")))
        gflow::parser::ResourceManager::resetWorkingDir(gflow::parser::ResourceManager::getWorkingDir());

    ImGui::Separator();

    if (!addTreeNode(IMGUI_NAME("root"), "")) return;

    const std::vector<std::string> orderedPaths = gflow::parser::ResourceManager::getTree().getOrderedPaths();
    for (const std::string& path : orderedPaths)
    {
        std::string name = gflow::string::getPathFilename(path);
        
        // Check if it is hidden
        {
            if (name[0] == '_') continue;
            std::vector<std::string> pathParts = gflow::string::split(path, "/");
            bool isHidden = false;
            for (const std::string& part : pathParts)
            {
                if (part[0] == '_')
                {
                    isHidden = true;
                    break;
                }
            }
            if (isHidden) continue;
        }

        if (!folderStack.empty() && path == folderStack.back().first)
        {
            popFolder(folderStack);
            continue;
        }

        std::string parentDir = gflow::string::getPathDirectory(path);
        if (!parentDir.empty() && !folderStack.back().second) 
            continue;

        if (path.back() == '/')
        {
            pushFolder(folderStack, path, name);
            continue;
        }

        if (gflow::parser::ResourceManager::hasResource(path) && (m_typeFilter.empty() || gflow::parser::ResourceManager::getResourceType(path) == m_typeFilter))
        {
            if (addSelectable(name, path == m_selectedResource) && path != m_selectedResource)
            {
                m_selectedResource = path;
                m_resourceSelectedSignal.emit(m_selectedResource);
            }
        }
    }
    ImGui::TreePop();
}

void ImGuiResourcesWindow::projectLoaded()
{
    m_selectedResource = "";
    m_resourceSelectedSignal.emit(m_selectedResource);
}
