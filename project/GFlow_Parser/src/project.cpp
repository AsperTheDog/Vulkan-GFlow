#include "project.hpp"

#include <fstream>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

#include "pipeline.hpp"
#include "string_helper.hpp"
#include "utils/logger.hpp"

namespace gflow::parser
{
    std::unordered_map<std::string, std::function<Resource* (std::string_view, Project*)>> Project::s_resourceFactories = {
            {"Pipeline", gflow::parser::Pipeline::create},
            {"RenderPass", nullptr},
            {"Image", nullptr},
            {"VertexBuffer", nullptr},
            {"IndexBuffer", nullptr},
            {"Buffer", nullptr}
    };

    Project::Project(const std::string_view name, const std::string_view workingDir) : m_name(name), m_workingDir(workingDir)
    {
    }

    Resource& Project::loadResource(const std::string& path)
    {
        std::string resolvedPath = m_workingDir.empty() ? path : m_workingDir + "/" + path;
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
        std::string content{ std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{} };
        const std::vector<std::string> lines = string::split(content, "\n");
        std::unordered_map<std::string, std::string> tokens;
        for (std::string line : lines)
        {
            std::erase_if(line, isspace);
            if (line[0] == '[')
            {
                std::string header = content.substr(0, content.find('\n'));
                std::erase_if(header, isspace);
                resourceType = string::reduce(header);
                continue;
            }
            std::pair<std::string, std::string> parts = string::tokenize(line, "=");
            if (parts.second.empty())
            {
                Logger::print("Invalid token" + parts.first + " in resource " + path + ", ignoring", Logger::WARN);
                continue;
            }
            tokens.insert(parts);
        }

        Resource& res = createResource(resourceType, resolvedPath);

        for (const auto& [key, value] : tokens)
        {
            res.set(key, value);
        }

        return res;
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
        if (!m_resources.contains(path))
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

    std::vector<std::string> Project::getResourcePaths(const std::string_view type)
    {
        std::vector<std::string> resources;
        for (const auto& [path, resource] : m_resources)
        {
            if (resource->getType() == type)
                resources.push_back(path);
        }
        return resources;
    }
}
