#pragma once
#include <functional>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "resource.hpp"

namespace gflow::parser
{
    class Project final : public Resource
    {
    public:
        explicit Project(std::string name, std::string_view workingDir);

        Resource& loadResource(const std::string& path);
        Resource& createResource(const std::string& type, const std::string& path);

        [[nodiscard]] Resource& getResource(const std::string& path) const;
        [[nodiscard]] Resource& getResource(uint32_t id) const;
        [[nodiscard]] bool hasResource(const std::string& path) const;

        [[nodiscard]] std::string getName() const { return m_name; }
        [[nodiscard]] std::string getWorkingDir() const { return m_workingDir; }

        std::vector<std::string> getResourcePaths(const std::string& type = "");

        [[nodiscard]] std::string getType() const override;

        [[nodiscard]] std::vector<std::string> getCustomExports() const override;
        std::pair<std::string, std::string> get(const std::string& variable) override;
        void set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies) override;

    private:
        std::string m_name;
        std::string m_workingDir;

        std::map<std::string, Resource*> m_resources;

        static std::unordered_map<std::string, std::function<Resource*(const std::string&, Project*)>> s_resourceFactories;
    };
}


