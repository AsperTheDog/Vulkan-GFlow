#include "core_project.hpp"

#include <fstream>


namespace gflow
{
	Project::Project(const std::string_view path, const uint32_t environment)
			: m_path(path), m_environment(environment)
	{
		std::ifstream file(m_path.data(), std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error(std::string("Failed to open project file: ") + path.data());
		}
		file.read(reinterpret_cast<char*>(&m_fileStructure.requirementsPos), sizeof(size_t));
		file.read(reinterpret_cast<char*>(&m_fileStructure.renderPassPos), sizeof(size_t));
		file.read(reinterpret_cast<char*>(&m_fileStructure.resourcesPos), sizeof(size_t));
		file.close();
	}

	Project::Requirements Project::getRequirements() const
	{
		Requirements requirements{};
		std::ifstream file(m_path.data(), std::ios::binary);
		file.seekg(m_fileStructure.requirementsPos);
		file.read(reinterpret_cast<char*>(&requirements.features), sizeof(VkPhysicalDeviceFeatures));
		file.read(reinterpret_cast<char*>(&requirements.queueFlags), sizeof(VkQueueFlags));
		file.read(reinterpret_cast<char*>(&requirements.present), sizeof(bool));
		uint16_t extensionCount;
		file.read(reinterpret_cast<char*>(&extensionCount), sizeof(extensionCount));
		for (uint16_t i = 0; i < extensionCount; ++i)
		{
			uint16_t size;
			file.read(reinterpret_cast<char*>(&size), sizeof(size));
			std::string extension(size, '\0');
			file.read(extension.data(), size);
			requirements.extensions.insert(extension);
		}
		file.close();
		return requirements;
	}

	void Project::Requirements::operator+=(const Requirements& other)
	{
		uint8_t* featurePtr = reinterpret_cast<uint8_t*>(&features);
		const uint8_t* otherFeaturePtr = reinterpret_cast<const uint8_t*>(&other.features);

		for (size_t i = 0; i < sizeof(VkPhysicalDeviceFeatures); ++i)
		{
			featurePtr[i] |= otherFeaturePtr[i];
		}

		present |= other.present;
		queueFlags |= other.queueFlags;

		for (const std::string& extension : other.extensions)
		{
			extensions.insert(extension);
		}
	}
} // namespace gflow