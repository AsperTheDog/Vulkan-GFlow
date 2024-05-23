#include "resource_manager.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>

#include "resources/pipeline.hpp"
#include "resources/project.hpp"
#include "string_helper.hpp"

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

    std::unordered_map<std::string, std::function<Resource* (const std::string&, const Resource::ExportData*)>> ResourceManager::s_resourceFactories = {
        {gflow::parser::Pipeline::getTypeStatic(), gflow::parser::Pipeline::create},
        {gflow::parser::PipelineVertexInputState::getTypeStatic(), gflow::parser::PipelineVertexInputState::create},
        {gflow::parser::PipelineInputAssemblyState::getTypeStatic(), gflow::parser::PipelineInputAssemblyState::create},
        {gflow::parser::PipelineRasterizationState::getTypeStatic(), gflow::parser::PipelineRasterizationState::create},
        {gflow::parser::PipelineDepthStencilState::getTypeStatic(), gflow::parser::PipelineDepthStencilState::create}
    };

    bool ResourceManager::hasResource(const std::string& path)
    {
        return m_resources.contains(path);
    }

    bool ResourceManager::hasResource(const uint32_t id)
    {
        for (const auto& [path, resource] : m_resources)
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

    void ResourceManager::saveAll()
    {
        for (Resource* resource : m_resources | std::views::values)
        {
            resource->serialize();
        }
    }

    void ResourceManager::resetWorkingDir(const std::string& path)
    {
        const std::string absPath = std::filesystem::absolute(path).generic_string();
        m_workingDir = absPath + "/";
        m_resources.clear();
        m_fileTree.reset();
        obtainResources(m_workingDir);
    }

    Resource& ResourceManager::loadResource(const std::string& path)
    {
        if (m_resources.contains(path))
            return *m_resources[path];

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
        return createResource(resourceType, path);
    }

    Resource& ResourceManager::createResource(const std::string& type, const std::string& path)
    {
        if (!s_resourceFactories.contains(type))
            throw std::runtime_error("Unknown resource type " + (type.empty() ? "<empty type>" : type));

        if (m_resources.contains(path))
            throw std::runtime_error("Resource already exists");

        if (s_resourceFactories[type] == nullptr)
            throw std::runtime_error("Resource type " + type + " is not implemented yet");

        m_resources[path] = s_resourceFactories[type](path, nullptr);
        m_fileTree.addPath(path);
        return *m_resources[path];
    }

    Project& ResourceManager::loadProject(const std::string& path)
    {
        if (m_resources.contains(path))
            return *dynamic_cast<Project*>(m_resources[path]);

        resetWorkingDir(string::getPathDirectory(path));
        m_resources[path] = Project::create(string::getPathFilename(path), nullptr);
        m_project = path;
        return *dynamic_cast<Project*>(m_resources[path]);
    }

    Project& ResourceManager::createProject(const std::string& name)
    {
        const std::string path = name + ".proj";
        if (m_resources.contains(path))
            throw std::runtime_error("Resource already exists");

        m_resources[path] = Project::create(path, nullptr);
        *dynamic_cast<Project*>(m_resources[path])->name = name;
        m_project = path;
        return *dynamic_cast<Project*>(m_resources[path]);
    }

    std::vector<std::string> ResourceManager::getResourceTypes()
    {
        std::vector<std::string> types{};
        types.reserve(s_resourceFactories.size());
        for (const auto& type : s_resourceFactories | std::views::keys)
            types.push_back(type);
        return types;
    }

    Resource& ResourceManager::getResource(const std::string& path)
    {
        if (!hasResource(path))
            throw std::runtime_error("Resource not found");
        return *m_resources.at(path);
    }

    Resource& ResourceManager::getResource(const uint32_t id)
    {
        for (Resource* resource : m_resources | std::views::values)
        {
            if (resource->getID() == id)
                return *resource;
        }
        throw std::runtime_error("Resource not found");
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
                    Logger::print(e.what(), Logger::LevelBits::ERR);
                }

            }
        }
        Logger::popContext();
    }
}
