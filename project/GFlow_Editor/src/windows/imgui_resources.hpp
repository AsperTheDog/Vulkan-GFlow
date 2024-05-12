#pragma once
#include <unordered_map>

#include "imgui_editor_window.hpp"

namespace gflow::parser
{
    class Project;
}

class FileTree
{
public:
    explicit FileTree(std::string path) : root(std::move(path)) {}

    void addPath(const std::string& path);
    void reset();

    [[nodiscard]] std::vector<std::string> getOrderedPaths() const;

private:
    struct FileDirectory
    {
        std::string name;
        std::vector<FileDirectory> directories;
        std::vector<std::string> files;

        explicit FileDirectory(std::string path) : name(std::move(path)) {}

        void addPath(const std::string& path);
        FileDirectory& getOrAddDirectory(const std::string& name);

        [[nodiscard]] std::vector<std::string> getOrderedPaths(const std::string& cumulatedPath) const;
    };
    
    FileDirectory root;
};


class ImGuiResourcesWindow final : public ImGuiEditorWindow
{
public:
    explicit ImGuiResourcesWindow(const std::string_view& name);

    void draw() override;
    void projectLoaded();

private:
    void getPaths(const std::string& current, const std::string& baseDir);

    void pushFolder(std::vector<std::string>& folderStack, const std::string& path, const std::string& folderName);
    void popFolder(std::vector<std::string>& folderStack) const;

    void addTreeNode(const std::string& name);

    std::unordered_map<std::string, bool> m_openedFolders;
    FileTree m_fileTree{"root"};

    std::string m_selectedResource;
};

