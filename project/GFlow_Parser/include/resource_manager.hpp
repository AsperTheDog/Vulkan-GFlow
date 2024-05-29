#pragma once
#include <functional>
#include <map>
#include <string>

#include "resource.hpp"

namespace gflow::parser
{
    class Project;

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

    class ResourceManager
    {
    public:
        static void resetWorkingDir(const std::string& path);

        static Resource& loadResource(const std::string& path);
        static Resource& createResource(const std::string& type, const std::string& path);

        static Project& loadProject(const std::string& path);
        static Project& createProject(const std::string& name);
        
        [[nodiscard]] static FileTree& getTree() { return m_fileTree; }
        
        [[nodiscard]] static std::vector<std::string> getResourceTypes();
        [[nodiscard]] static Resource& getResource(const std::string& path);
        [[nodiscard]] static Resource& getResource(uint32_t id);
        [[nodiscard]] static bool hasResource(const std::string& path);
        [[nodiscard]] static bool hasResource(uint32_t id);
        [[nodiscard]] static std::string getResourceType(const std::string& path);

        [[nodiscard]] static std::string getWorkingDir() { return m_workingDir; }
        [[nodiscard]] static Resource& getProject() { return *m_resources[m_project]; }
        [[nodiscard]] static std::string getProjectPath() { return m_workingDir + m_project; }
        [[nodiscard]] static bool hasProject() { return !m_project.empty(); }

        static bool isTypeSubresource(const std::string& type);

        static std::vector<std::string> getResourcePaths(const std::string& type = "");

        static void saveAll();

    private:
        static void obtainResources(const std::string& current);

        inline static std::map<std::string, Resource*> m_resources;
        inline static std::string m_project;
        inline static std::string m_workingDir;

        inline static FileTree m_fileTree{ "root" };

        static std::unordered_map<std::string, std::function<Resource* (const std::string&, Resource::ExportData*)>> s_resourceFactories;
    };
}

