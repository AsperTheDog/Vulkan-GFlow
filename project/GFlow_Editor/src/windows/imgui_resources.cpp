#include "imgui_resources.hpp"

#include <filesystem>

#include "editor.hpp"
#include "string_helper.hpp"


std::vector<std::string> FileTree::FileDirectory::getOrderedPaths(const std::string& cumulatedPath) const
{
    std::vector<std::string> paths;
    for (const FileDirectory& directory : directories)
    {
        std::string path = cumulatedPath + directory.name + "/";
        paths.push_back(path);
        std::vector<std::string> subPaths = directory.getOrderedPaths(path);
        for (const std::string& file : subPaths)
            paths.push_back(file);
        paths.push_back(path);
    }
    for (const std::string& file : files)
        paths.push_back(cumulatedPath + file);
    return paths;
}

void FileTree::FileDirectory::addPath(const std::string& path)
{
    const std::vector<std::string> splitPath = gflow::string::split(path, "/");
    const bool isDirectory = path.back() == '/';
    if (splitPath.size() == 1)
    {
        if (isDirectory)
            directories.emplace_back(splitPath[0]);
        else
            files.push_back(splitPath[0]);
    }
    else
    {
        FileDirectory& directory = getOrAddDirectory(splitPath[0]);
        directory.addPath(gflow::string::join(splitPath, "/", 1) + (isDirectory ? "/" : ""));
    }
}

FileTree::FileDirectory& FileTree::FileDirectory::getOrAddDirectory(const std::string& name)
{
    for (FileDirectory& directory : directories)
    {
        if (directory.name == name)
            return directory;
    }
    directories.emplace_back(name);
    return directories.back();
}

std::vector<std::string> FileTree::getOrderedPaths() const
{
    return root.getOrderedPaths("");
}

void FileTree::addPath(const std::string& path)
{
    root.addPath(path);
}

void FileTree::removePath(const std::string& path)
{
    root.removePath(path);
}

void FileTree::renamePath(const std::string& path, const std::string& newName)
{
    root.renamePath(path, newName);
}

void FileTree::FileDirectory::removePath(const std::string& path)
{
    const std::vector<std::string> splitPath = gflow::string::split(path, "/");
    const bool isDirectory = path.back() == '/';
    if (splitPath.size() == 1)
    {
        if (isDirectory)
        {
            for (auto it = directories.begin(); it != directories.end(); ++it)
            {
                if (it->name == splitPath[0])
                {
                    directories.erase(it);
                    return;
                }
            }
        }
        else
        {
            const auto it = std::ranges::find(files, splitPath[0]);
            if (it != files.end())
                files.erase(it);
        }
    }
    else
    {
        for (FileDirectory& directory : directories)
        {
            if (directory.name == splitPath[0])
            {
                directory.removePath(gflow::string::join(splitPath, "/", 1) + (isDirectory ? "/" : ""));
                return;
            }
        }
    }
}

void FileTree::FileDirectory::renamePath(const std::string& path, const std::string& newName)
{
    const std::vector<std::string> splitPath = gflow::string::split(path, "/");
    const bool isDirectory = path.back() == '/';
    if (splitPath.size() == 1)
    {
        if (isDirectory)
        {
            for (FileDirectory& directory : directories)
            {
                if (directory.name == splitPath[0])
                {
                    directory.name = newName;
                    return;
                }
            }
        }
        else
        {
            const auto it = std::ranges::find(files, splitPath[0]);
            if (it != files.end())
                *it = newName;
        }
    }
    else
    {
        for (FileDirectory& directory : directories)
        {
            if (directory.name == splitPath[0])
            {
                directory.renamePath(gflow::string::join(splitPath, "/", 1) + (isDirectory ? "/" : ""), newName);
                return;
            }
        }
    }
}

void FileTree::reset()
{
    root = FileDirectory(root.name);
}

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

        }
        ImGui::EndPopup();
    }
    return opened;
}

void ImGuiResourcesWindow::addSelectable(const std::string& name) const
{
    ImGui::Selectable(name.c_str());
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
}

void ImGuiResourcesWindow::draw()
{
    if (!Editor::hasProject()) return;
    const gflow::parser::Project& project = Editor::getProject();

    std::vector<std::pair<std::string, bool>> folderStack;

    ImGui::Begin(m_name.c_str(), &open);

    if (ImGui::Button("Refresh")) projectLoaded();

    ImGui::Separator();

    if (!addTreeNode("root", ""))
    {
        ImGui::End();
        return;
    }

    const std::vector<std::string> orderedPaths = m_fileTree.getOrderedPaths();
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
        if (project.hasResource(path) && ImGui::Selectable(name.c_str(), path == m_selectedResource) && path != m_selectedResource)
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
    m_fileTree.reset();
    getPaths(Editor::getProject().getWorkingDir(), Editor::getProject().getWorkingDir());
}

void ImGuiResourcesWindow::folderCreated(const std::string& path)
{
    m_fileTree.addPath(path);
}

void ImGuiResourcesWindow::folderDeleted(const std::string& path)
{
    m_fileTree.removePath(path);
}

void ImGuiResourcesWindow::folderRenamed(const std::string& path, const std::string& newName)
{
    m_fileTree.renamePath(path, newName);
}

void ImGuiResourcesWindow::resourceCreated(const std::string& path)
{
    m_fileTree.addPath(path);
}

void ImGuiResourcesWindow::resourceDeleted(const std::string& path)
{
    m_fileTree.removePath(path);
}

void ImGuiResourcesWindow::resourceRenamed(const std::string& path, const std::string& newName)
{
    m_fileTree.renamePath(path, newName);
}

void ImGuiResourcesWindow::getPaths(const std::string& current, const std::string& baseDir)
{
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(current))
    {
        m_fileTree.addPath(std::filesystem::relative(entry.path(), baseDir).generic_string() + (entry.is_directory() ? "/" : ""));
        if (entry.is_directory())
            getPaths(entry.path().generic_string(), baseDir);
    }
}
