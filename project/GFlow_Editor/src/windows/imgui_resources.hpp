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
    void removePath(const std::string& path);
    void renamePath(const std::string& path, const std::string& newName);
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
        void removePath(const std::string& path);
        void renamePath(const std::string& path, const std::string& newName);

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

    void folderCreated(const std::string& path);
    void folderDeleted(const std::string& path);
    void folderRenamed(const std::string& path, const std::string& newName);
    void resourceCreated(const std::string& path);
    void resourceDeleted(const std::string& path);
    void resourceRenamed(const std::string& path, const std::string& newName);

private:
    void getPaths(const std::string& current, const std::string& baseDir);

    void pushFolder(std::vector<std::pair<std::string, bool>>& folderStack, const std::string& path, const std::string& folderName) const;
    void popFolder(std::vector<std::pair<std::string, bool>>& folderStack) const;

    [[nodiscard]] bool addTreeNode(const std::string& name, const std::string& path) const;
    void addSelectable(const std::string& name) const;

    FileTree m_fileTree{ "root" };

    std::string m_selectedResource;
};

