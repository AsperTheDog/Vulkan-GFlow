#include "project.hpp"

#include <stdexcept>

namespace gflow
{
    Project::Project() : Serializable("Project", "proj")
    {

    }

    std::string Project::getSerialized(std::string_view key) const
    {

        throw std::runtime_error("Invalid key in get");
    }

    void Project::set(std::string_view key, std::string_view value)
    {

    }

    std::vector<std::string> Project::keys() const
    {
        return {};
    }

    bool Project::isSubresource(std::string_view key) const
    {
        return false;
    }

    Serializable* Project::getSubresource(std::string_view key)
    {
        return nullptr;
    }

    Project Project::load(const std::string_view path)
    {
        Project project{};
        Serialization::deserialize(project, path);
        return project;
    }
}
