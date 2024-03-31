#include "project.hpp"

#include <stdexcept>

namespace gflow
{
    Project::Project() : Serializable("proj")
    {

    }

    std::string Project::getSerialized(const std::string_view key) const
    {

        throw std::runtime_error(std::string("Invalid key ") + key.data() + " in get for serializable of type " + m_suffix);
    }

    void Project::set(std::string_view key, std::string_view value)
    {

    }

    void Project::serialize(const std::string_view filename) const
    {
        Serialization::serialize(*this, filename);
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

    void Project::addSubresource(std::string_view key, Serialization::ResourceData& subresource)
    {

    }

    Project Project::load(const std::string_view path)
    {
        Project project{};
        Serialization::deserialize(project, path);
        return project;
    }
}
