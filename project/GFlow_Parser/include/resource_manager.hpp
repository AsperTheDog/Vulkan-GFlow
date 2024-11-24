#pragma once
#include <functional>
#include <map>
#include <string>

#include "resource.hpp"

#define DECLARE_PUBLIC_RESOURCE_ANCESTOR(type, parent)                                               \
    DECLARE_RESOURCE_ANCESTOR(type, parent)                                                          \
    inline static gflow::parser::PublicResourceDeclarationFactory<type> __##type##Factory{};

#define DECLARE_PUBLIC_RESOURCE_ANCESTOR_NO_CONST(type, parent)                                      \
    DECLARE_RESOURCE_ANCESTOR_NO_CONST(type, parent)                                                 \
    inline static gflow::parser::PublicResourceDeclarationFactory<type> __##type##Factory{};

#define DECLARE_PRIVATE_RESOURCE_ANCESTOR(type, parent)                                              \
    DECLARE_RESOURCE_ANCESTOR(type, parent)                                                          \
    inline static gflow::parser::PrivateResourceDeclarationFactory<type> __##type##Factory{};

#define DECLARE_PRIVATE_RESOURCE_ANCESTOR_NO_CONST(type, parent)                                     \
    DECLARE_RESOURCE_ANCESTOR_NO_CONST(type, parent)                                                 \
    inline static gflow::parser::PrivateResourceDeclarationFactory<type> __##type##Factory{};

#define DECLARE_PUBLIC_RESOURCE(type)                                                                \
    DECLARE_PUBLIC_RESOURCE_ANCESTOR(type, Resource)

#define DECLARE_PUBLIC_RESOURCE_NO_CONST(type)                                                       \
    DECLARE_PUBLIC_RESOURCE_ANCESTOR_NO_CONST(type, Resource)

#define DECLARE_PRIVATE_RESOURCE(type)                                                               \
    DECLARE_PRIVATE_RESOURCE_ANCESTOR(type, Resource)

#define DECLARE_PRIVATE_RESOURCE_NO_CONST(type)                                                      \
    DECLARE_PRIVATE_RESOURCE_ANCESTOR_NO_CONST(type, Resource)


namespace gflow::parser
{
    class Project;

    class FileTree
    {
    public:
        explicit FileTree(std::string path) : root(std::move(path)) {}

        void addPath(const std::string& path);
        void removePath(const std::string& path);
        void deletePath(const std::string& path, const std::string& workingDir);
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

            void deleteFromOS(const std::string& parentPath);
        };

        FileDirectory root;
    };

    class ResourceManager
    {
    public:
        static void resetWorkingDir(const std::string& path);

        static Resource* loadResource(const std::string& path);

        static Resource* createResource(const std::string& type, const std::string& path, Resource::ExportData* data = nullptr);
        template <typename T> static T* createResource(const std::string& path, Resource::ExportData* data = nullptr);
        static Resource* createResource(const std::string& path, const Resource::ResourceFactory& factory, Resource::ExportData* data = nullptr);

        static bool deleteResource(const std::string& path);
        static bool deleteResource(Resource* resource);


        static Project* loadProject(const std::string& path);
        static Project* createProject(const std::string& name);
        
        [[nodiscard]] static FileTree& getTree() { return m_fileTree; }
        
        [[nodiscard]] static std::vector<std::string> getResourceTypes();
        [[nodiscard]] static Resource* getResource(const std::string& path);
        [[nodiscard]] static Resource* getResource(uint32_t id);
        [[nodiscard]] static Resource* getMetaresource(const std::string& path);
        [[nodiscard]] static Resource* getMetaresource(uint32_t id);
        [[nodiscard]] static gflow::parser::Resource* getSubresource(const std::string& path, const std::string& subpath);
        [[nodiscard]] static bool hasResource(const std::string& path);
        [[nodiscard]] static bool hasResource(uint32_t id);
        [[nodiscard]] static std::string getResourceType(const std::string& path);

        [[nodiscard]] static std::string getWorkingDir() { return m_workingDir; }
        [[nodiscard]] static Resource* getProject() { return m_resources[m_project]; }
        [[nodiscard]] static std::string getProjectPath() { return m_workingDir + m_project; }
        [[nodiscard]] static bool hasProject() { return !m_project.empty(); }

        static bool isTypeSubresource(const std::string& type);

        static std::vector<std::string> getResourcePaths(const std::string& type = "");
        static std::string makePathAbsolute(const std::string& path);
        static void deleteDirectory(const std::string& string);

        static void saveAll();

        static bool injectResourceFactory(const std::string& type, const Resource::ResourceFactory& factory, bool isPrivate);
        [[nodiscard]] static bool hasResourceFactory(const std::string& type);

    private:
        static void obtainResources(const std::string& current);

        inline static std::map<std::string, Resource*> m_resources;
        inline static std::vector<Resource*> m_embeddedResources;
        inline static std::string m_project;
        inline static std::string m_workingDir;

        inline static FileTree m_fileTree{ "root" };

        inline static std::unordered_map<std::string, std::pair<Resource::ResourceFactory, bool>> s_resourceFactories{};
    };

    template <typename T>
    class PublicResourceDeclarationFactory
    {
    public:
        PublicResourceDeclarationFactory()
        {
            ResourceManager::injectResourceFactory(T::getTypeStatic(), &T::create, false);
        }
    };

    template <typename T>
    class PrivateResourceDeclarationFactory
    {
    public:
        PrivateResourceDeclarationFactory()
        {
            ResourceManager::injectResourceFactory(T::getTypeStatic(), &T::create, true);
        }
    };

    template <typename T>
    T* ResourceManager::createResource(const std::string& path, Resource::ExportData* data)
    {
        static_assert(std::is_base_of_v<Resource, T>, "T must be a subclass of Resource");
        Resource* elem = createResource(path, Resource::create<T>, data);
        if (!elem->isSubresource())
            elem->serialize();
        return dynamic_cast<T*>(elem);
    }
}

