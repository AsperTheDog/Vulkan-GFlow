#include "project.hpp"

#include <filesystem>
#include <stdexcept>

#include "pipeline.hpp"

namespace gflow::parser
{
    Project::Project(const std::string& path) : Resource(path) {}

    Resource* Project::create(const std::string& path, const ExportData* metadata)
    {
        Project* project = new Project(path);
        if (!project->deserialize())
            project->serialize();
        return project;
    }
}
