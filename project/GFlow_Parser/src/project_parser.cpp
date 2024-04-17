#include "project_parser.hpp"

#include "serialization/components/project.hpp"

namespace gflow
{
	void ProjectParser::exportProject(const std::string_view path, std::string_view name)
	{
		Project project = Project::load(path);
	}

}
