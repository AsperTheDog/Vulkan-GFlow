#pragma once
#include <string>
#include <unordered_set>
#include <vulkan/vulkan.h>

#include "utils/identifiable.hpp"

namespace gflow
{
	class Project : public Identifiable
	{
	public:
		struct Requirements {
			VkPhysicalDeviceFeatures features{};
			VkQueueFlags queueFlags{};
			bool present = false;
			std::unordered_set<std::string> extensions{};

			void operator+=(const Requirements& other);
		};

	private:
		struct FileStructure;

		Project(std::string_view path, uint32_t environment);

		[[nodiscard]] Requirements getRequirements() const;

		std::string m_path;
		uint32_t m_environment;

		friend class Environment;

		struct FileStructure
		{
			size_t requirementsPos = 0;
			size_t renderPassPos = 0;
			size_t resourcesPos = 0;
		} m_fileStructure;
	};
} // namespace gflow

