#include "project.hpp"

#include <stdexcept>

namespace gflow
{
    Project::Project() : Serializable("proj")
    {
        ConfigModule module[5] = {ConfigModule{}, ConfigModule{}, ConfigModule{}, ConfigModule{}, ConfigModule{}};
        for (auto& entry : module)
        {
            entry.set("gpu", std::to_string(rand()));
            m_entries.getRef().push_back(entry);
        }
    }

    Project::Project(const std::string_view filename) : Serializable("proj")
    {
        Serialization::deserialize(*this, filename);
        ConfigModule module[5] = {ConfigModule{}, ConfigModule{}, ConfigModule{}, ConfigModule{}, ConfigModule{}};
        for (auto& entry : module)
        {
            entry.set("gpu", std::to_string(rand()));
            m_entries.getRef().push_back(entry);
        }
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
        return m_entries.doesNameMatch(key);
    }

    Serializable* Project::getSubresource(const std::string_view key, const bool willEdit)
    {
        if (m_entries.doesNameMatch(key))
        {
            return &m_entries.getRef();
        }
        return nullptr;
    }

    Project Project::load(const std::string_view path)
    {
        Project project{};
        Serialization::deserialize(project, path);
        return project;
    }
}
