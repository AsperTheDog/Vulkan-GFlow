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

void FileTree::reset()
{
    root = FileDirectory(root.name);
}

ImGuiResourcesWindow::ImGuiResourcesWindow(const std::string_view& name)
    : ImGuiEditorWindow(name)
{
}

void ImGuiResourcesWindow::pushFolder(std::vector<std::string>& folderStack, const std::string& path, const std::string& folderName)
{
    folderStack.emplace_back(path);
    m_openedFolders[path] = ImGui::TreeNode(folderName.c_str());
}

void ImGuiResourcesWindow::popFolder(std::vector<std::string>& folderStack) const
{
    if (m_openedFolders.at(folderStack.back()))
    {
        ImGui::TreePop();
    }
    folderStack.pop_back();
}

void ImGuiResourcesWindow::addTreeNode(const std::string& name)
{
    if (!m_openedFolders.contains(name))
    {
        m_openedFolders[name] = false;
    }
    m_openedFolders[name] = ImGui::TreeNode(name.c_str());
    //if (ImGui)
}

void ImGuiResourcesWindow::draw()
{
    if (!Editor::hasProject()) return;
    gflow::parser::Project& project = Editor::getProject();

    std::vector<std::string> folderStack;

    ImGui::Begin(m_name.c_str(), &open);
    if (!ImGui::TreeNode("root"))
    {
        ImGui::End();
        return;
    }

    std::vector<std::string> projectPaths = project.getResourcePaths();
    const std::vector<std::string> orderedPaths = m_fileTree.getOrderedPaths();
    for (const std::string& path : orderedPaths)
    {
        std::string name = gflow::string::getPathFilename(path);
        std::string parentDir = gflow::string::getPathDirectory(path);
        if (!folderStack.empty() && path == folderStack.back())
        {
            popFolder(folderStack);
            continue;
        }
        if (!parentDir.empty() && !m_openedFolders[parentDir + "/"]) continue;

        if (path.back() == '/')
        {
            pushFolder(folderStack, path, name);
            continue;
        }
        if (ImGui::Selectable(name.c_str(), path == m_selectedResource) && path != m_selectedResource)
        {
            std::cout << "Selected: " << path << std::endl;
            m_selectedResource = path;
            continue;
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

void ImGuiResourcesWindow::getPaths(const std::string& current, const std::string& baseDir)
{
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(current))
    {
        m_fileTree.addPath(std::filesystem::relative(entry.path(), baseDir).generic_string() + (entry.is_directory() ? "/" : ""));
        if (entry.is_directory())
            getPaths(entry.path().generic_string(), baseDir);
    }
}
