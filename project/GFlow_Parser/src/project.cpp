#include "project.hpp"

#include <filesystem>
#include <stdexcept>

#include "pipeline.hpp"

namespace gflow::parser
{
    Project::Project(const std::string& path) : Resource(path) {}

    std::string Project::getType() const
    {
        return "Project";
    }

    Resource* Project::create(const std::string& path)
    {
        Project* project = new Project(path);
        if (!project->deserialize())
            project->serialize();
        return project;
    }
}
