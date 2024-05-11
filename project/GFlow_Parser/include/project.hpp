#pragma once
#include <functional>
#include <map>
#include <unordered_map>

#include "resource.hpp"

namespace gflow::parser
{
    class Project
    {
    public:
        explicit Project(std::string_view name, std::string_view workingDir);

        Resource& loadResource(const std::string& path);
        Resource& createResource(const std::string& type, const std::string& path);

        [[nodiscard]] Resource& getResource(const std::string& path) const;
        [[nodiscard]] Resource& getResource(uint32_t id) const;

        std::vector<std::string> getResourcePaths(std::string_view type);

    private:
        std::string m_name;
        std::string m_workingDir;

        std::map<std::string, Resource*> m_resources;

        static std::unordered_map<std::string, std::function<Resource*(std::string_view, Project*)>> s_resourceFactories;
    };
}


