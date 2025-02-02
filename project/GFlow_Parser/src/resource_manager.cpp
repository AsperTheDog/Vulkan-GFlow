#include "resource_manager.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>

#include "resources/project.hpp"
#include "string_helper.hpp"
#include "resources/renderpass.hpp"

namespace gflow::parser
{

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

    void FileTree::FileDirectory::deleteFromOS(const std::string& parentPath)
    {
        for (FileDirectory& directory : directories)
            directory.deleteFromOS(parentPath + name + "/");
        for (const std::string& file : files)
        {
            const std::string path = parentPath + file;
            if (std::filesystem::exists(path))
                std::filesystem::remove(path);
        }
    }

    void FileTree::FileDirectory::addPath(const std::string& path)
    {
        const std::vector<std::string> splitPath = gflow::string::split(path, "/");
        const bool isDirectory = path.back() == '/';
        if (splitPath.size() == 1)
        {
            if (isDirectory)
                getOrAddDirectory(splitPath[0]);
            else if (std::ranges::find(files, splitPath[0]) == files.end())
                files.push_back(splitPath[0]);
        }
        else
        {
            FileDirectory& directory = getOrAddDirectory(splitPath[0]);
            directory.addPath(gflow::string::join(splitPath, "/", 1) + (isDirectory ? "/" : ""));
        }
    }

    FileTree::FileDirectory& FileTree::FileDirectory::getOrAddDirectory(const std::string& dirName)
    {
        for (FileDirectory& directory : directories)
        {
            if (directory.name == dirName)
                return directory;
        }
        directories.emplace_back(dirName);
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

    void FileTree::deletePath(const std::string& path, const std::string& workingDir)
    {
        root.removePath(path);

        if (!std::filesystem::exists(workingDir + path))
            std::filesystem::remove_all(workingDir + path);
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
    
    bool ResourceManager::hasResource(const std::string& path)
    {
        return m_resources.contains(path);
    }

    bool ResourceManager::hasResource(const uint32_t id)
    {
        for (const Resource* resource : m_resources | std::views::values)
        {
            if (resource->getID() == id)
                return true;
        }
        return false;
    }

    std::string ResourceManager::getResourceType(const std::string& path)
    {
        if (!hasResource(path))
            return "";
        return m_resources.at(path)->getType();
    }

    bool ResourceManager::isTypeSubresource(const std::string& type)
    {
        return !s_resourceFactories.contains(type);
    }

    std::vector<std::string> ResourceManager::getResourcePaths(const std::string& type)
    {
        std::vector<std::string> resources;
        for (const auto& [path, resource] : m_resources)
        {
            if (resource->getType() == type)
                resources.push_back(path);
        }
        return resources;
    }

    std::string ResourceManager::makePathAbsolute(const std::string& path)
    {
        return m_workingDir + path;
    }

    void ResourceManager::deleteDirectory(const std::string& string)
    {
        //Find all resources contained in this folder
        for (const std::string& path : m_fileTree.getOrderedPaths())
        {
            if (path.starts_with(string) && hasResource(path))
                deleteResource(path);
        }
        m_fileTree.deletePath(string, m_workingDir);
    }

    void ResourceManager::saveAll()
    {
        for (Resource* resource : m_resources | std::views::values)
        {
            resource->serialize();
        }
    }

    bool ResourceManager::injectResourceFactory(const std::string& type, const Resource::ResourceFactory& factory, bool isPrivate)
    {
        if (s_resourceFactories.contains(type))
            return false;
        s_resourceFactories[type] = {factory, isPrivate};
        return true;
    }

    bool ResourceManager::hasResourceFactory(const std::string& type)
    {
        return s_resourceFactories.contains(type);
    }

    bool ResourceManager::isResourcePublic(const std::string& path)
    {
        if (!hasResource(path))
            return false;
        return !s_resourceFactories[m_resources[path]->getType()].second;
    }

    gflow::parser::Resource* ResourceManager::getSubresource(const std::string& path, const std::string& subpath)
    {
        if (!hasResource(path))
            return nullptr;

        Resource* res = getResource(path);
        std::vector<std::string> pathSteps = gflow::string::split(subpath, ".");
        pathSteps.erase(pathSteps.begin());
        for (const std::string& pathStep : pathSteps)
        {
            res = res->getValue<Resource *>(pathStep);
        }
        return res;
    }

    void ResourceManager::resetWorkingDir(const std::string& path)
    {
        const std::string absPath = std::filesystem::absolute(path).generic_string();
        m_workingDir = absPath + "/";

        for (auto& [_, resource] : m_resources)
            delete resource;
        for (const Resource* resource : m_embeddedResources)
            delete resource;

        m_resources.clear();
        m_embeddedResources.clear();
        m_fileTree.reset();
        obtainResources(m_workingDir);
    }

    Resource* ResourceManager::loadResource(const std::string& path)
    {
        if (m_resources.contains(path))
            return m_resources[path];

        std::string resolvedPath = m_workingDir + path;

        std::ifstream file{ resolvedPath };
        if (!file.is_open() && !m_workingDir.empty())
        {
            resolvedPath = path;
            file.open(resolvedPath);
        }

        if (!file.is_open())
            throw std::runtime_error("Failed to open resource file " + path);

        std::string resourceType;
        for (std::string line; std::getline(file, line);)
        {
            if (line.empty()) continue;

            std::erase_if(line, isspace);
            if (line[0] == '[')
            {
                line = string::reduce(line);
                std::string type;
                bool isMain = false;
                for (const std::string& part : string::split(line, ","))
                {
                    const auto [key, value] = string::tokenize(part, "=");
                    if (key == "type") type = value;
                    else if (key == "level") isMain = value != "Subresource";
                }
                if (isMain)
                {
                    resourceType = type;
                    break;
                }
            }
        }
        Resource* elem = createResource(resourceType, path);
        elem->deserialize();
        return elem;
    }

    Resource* ResourceManager::createResource(const std::string& type, const std::string& path, Resource::ExportData* data)
    {
        if (!s_resourceFactories.contains(type))
            throw std::runtime_error("Unknown resource type " + (type.empty() ? "<empty type>" : type) + " while parsing file '" + path + "'");

        if (s_resourceFactories[type].first == nullptr)
            throw std::runtime_error("Resource type " + type + " is not implemented yet");

        return createResource(path, s_resourceFactories[type].first, data);
    }

    Resource* ResourceManager::createResource(const std::string& path, const Resource::ResourceFactory& factory, Resource::ExportData* data)
    {
        if (path.empty())
        {
            m_embeddedResources.push_back(factory("", data));
            Resource* res = m_embeddedResources.back();
            return res;
        }

        if (m_resources.contains(path))
            throw std::runtime_error("Resource already exists");

        m_resources[path] = factory(path, data);
        m_fileTree.addPath(path);
        return m_resources[path];
    }

    bool ResourceManager::deleteResource(const std::string& path)
    {
        return deleteResource(getResource(path));
    }

    bool ResourceManager::deleteResource(Resource* resource)
    {
        if (resource == nullptr)
            return false;

        for (auto it = m_embeddedResources.begin(); it != m_embeddedResources.end(); ++it)
        {
            if (*it != resource)
                continue;
            
            m_embeddedResources.erase(it);
            for (const Resource::ExportData& subresource : resource->getExports())
            {
                if (subresource.type != RESOURCE || subresource.data == nullptr) 
                    continue;

                Resource* sub = *static_cast<Resource**>(subresource.data);
                if (sub != nullptr && sub->isSubresource())
                    deleteResource(sub);
            }
            delete resource;
            return true;
        }
        return false;
    }

    Project* ResourceManager::loadProject(const std::string& path)
    {
        if (m_resources.contains(path))
            return dynamic_cast<Project*>(m_resources[path]);

        resetWorkingDir(string::getPathDirectory(path));
        m_resources[path] = Resource::create<Project>(string::getPathFilename(path), nullptr);
        m_project = path;
        return dynamic_cast<Project*>(m_resources[path]);
    }

    Project* ResourceManager::createProject(const std::string& name)
    {
        const std::string path = name + ".proj";
        if (m_resources.contains(path))
            throw std::runtime_error("Resource already exists");

        m_resources[path] = Resource::create<Project>(path, nullptr);
        *dynamic_cast<Project*>(m_resources[path])->name = name;
        m_project = path;
        return dynamic_cast<Project*>(m_resources[path]);
    }

    std::vector<std::string> ResourceManager::getResourceTypes()
    {
        std::vector<std::string> types{};
        types.reserve(s_resourceFactories.size());
        for (const auto& [type, data] : s_resourceFactories)
        {
            if (!data.second)
                types.push_back(type);
        }
        return types;
    }

    Resource* ResourceManager::getResource(const std::string& path)
    {
        if (!hasResource(path))
            throw std::runtime_error("Resource not found");
        return m_resources.at(path);
    }

    Resource* ResourceManager::getResource(const uint32_t id)
    {
        for (Resource* resource : m_resources | std::views::values)
        {
            if (resource->getID() == id)
                return resource;
        }
        throw std::runtime_error("Resource not found");
    }

    Resource* ResourceManager::getMetaresource(const std::string& path)
    {
        return getResource(gflow::string::getPathDirectory(path) + "_" + gflow::string::getPathFilename(path) + ".meta");
    }

    Resource* ResourceManager::getMetaresource(const uint32_t id)
    {
        const std::string path = getResource(id)->getPath();
        return getMetaresource(path);
    }

    void ResourceManager::obtainResources(const std::string& current)
    {
        Logger::pushContext("Populate filesystem");
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(current))
        {
            std::string path = std::filesystem::relative(entry.path(), ResourceManager::getWorkingDir()).generic_string();
            m_fileTree.addPath(path + (entry.is_directory() ? "/" : ""));
            if (entry.is_directory())
                obtainResources(entry.path().generic_string());
            else
            {
                try
                {
                    loadResource(path);
                }
                catch (const std::exception& e)
                {
                    Logger::print(Logger::ERR, e.what());
                }

            }
        }
        Logger::popContext();
    }
}
