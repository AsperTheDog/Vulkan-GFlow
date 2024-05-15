#include "project.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

#include "pipeline.hpp"
#include "string_helper.hpp"
#include "utils/logger.hpp"

namespace gflow::parser
{
    std::unordered_map<std::string, std::function<Resource* (const std::string&, Project*)>> Project::s_resourceFactories = {
            {"Pipeline", gflow::parser::Pipeline::create},
            {"RenderPass", nullptr},
            {"Image", nullptr},
            {"VertexBuffer", nullptr},
            {"IndexBuffer", nullptr},
            {"Buffer", nullptr}
    };

    Project::Project(std::string name, const std::string_view workingDir) : m_name(std::move(name)), m_workingDir(workingDir)
    {
        if (!m_workingDir.empty())
        {
            if (m_workingDir.back() != '/')
                m_workingDir += '/';
            const std::filesystem::path dir{ m_workingDir };
            if (!std::filesystem::exists(dir))
                std::filesystem::create_directories(dir);
        }
    }

    Resource& Project::loadResource(const std::string& path)
    {
        std::string resolvedPath = m_workingDir + path;
        std::ifstream file{ resolvedPath };
        if (!file.is_open() && !m_workingDir.empty())
        {
            resolvedPath = path;
            file.open(resolvedPath);
        }

        if (!file.is_open())
            throw std::runtime_error("Failed to open resource file " + path);

        if (m_resources.contains(resolvedPath))
            return *m_resources[resolvedPath];

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

    Resource& Project::createResource(const std::string& type, const std::string& path)
    {
        if (!s_resourceFactories.contains(type))
            throw std::runtime_error("Unknown resource type " + (type.empty() ? "<empty type>" : type));

        if (m_resources.contains(path))
            throw std::runtime_error("Resource already exists");

        if (s_resourceFactories[type] == nullptr)
            throw std::runtime_error("Resource type " + type + " is not implemented yet");

        m_resources[path] = s_resourceFactories[type](path, this);
        return *m_resources[path];
    }

    Resource& Project::getResource(const std::string& path) const
    {
        if (!hasResource(path))
            throw std::runtime_error("Resource not found");
        return *m_resources.at(path);
    }

    Resource& Project::getResource(const uint32_t id) const
    {
        for (Resource* resource : m_resources | std::views::values)
        {
            if (resource->getID() == id)
                return *resource;
        }
        throw std::runtime_error("Resource not found");
    }

    std::vector<std::string> Project::getResourcePaths(const std::string& type)
    {
        std::vector<std::string> resources;
        for (const auto& [path, resource] : m_resources)
        {
            if (resource->getType() == type)
                resources.push_back(path);
        }
        return resources;
    }

    std::string Project::getType() const
    {
        return "Project";
    }

    std::vector<std::string> Project::getCustomExports() const
    {
        std::vector<std::string> exports;
        for (const auto& resource : m_resources | std::views::values)
        {
            exports.push_back("resources" + std::to_string(resource->getID()));
        }
        return exports;
    }

    std::pair<std::string, std::string> Project::get(const std::string& variable)
    {
        if (string::contains(variable, "resources"))
        {
            if (!string::contains(variable, "/"))
            {
                uint32_t id = 0;
                try
                {
                    id = std::stoi(string::tokenize(variable, "/").second);
                    return {getResource(id).getPath(), ""};
                }
                catch (const std::exception& e)
                {
                    Logger::print("Failed to parse resource index: " + std::string(e.what()), Logger::ERR);
                    return {"", ""};
                }
            }
        }
        return Resource::get(variable);
    }

    void Project::set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies)
    {
        Resource::set(variable, value, dependencies);
    }

    bool Project::hasResource(const std::string& path) const
    {
        return m_resources.contains(path);
    }
}
