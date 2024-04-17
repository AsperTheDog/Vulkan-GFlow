#include "serialization/components/project.hpp"

#include <stdexcept>

Project::Project() : Serializable("proj")
{

}

Project::Project(const std::string_view filename) : Serializable("proj")
{
    Serialization::deserialize(*this, filename);
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

bool Project::isSubresource(const std::string_view key) const
{
    return false;
}

Serializable* Project::getSubresource(const std::string_view key, const bool willEdit)
{
    return nullptr;
}

Project Project::load(const std::string_view path)
{
    Project project{};
    Serialization::deserialize(project, path);
    return project;
}

