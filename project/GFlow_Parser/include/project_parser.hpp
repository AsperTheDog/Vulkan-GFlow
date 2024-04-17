# pragma once
#include <string_view>

namespace gflow
{
	class ProjectParser
	{
		static void exportProject(std::string_view path, std::string_view name);
	};
}