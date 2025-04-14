#include "context.hpp"

#include <stdexcept>

#include "vulkan_context.hpp"

namespace gflow
{
	void Context::initVulkan(std::span<const char*> vulkanInstanceExtensions)
	{
#ifndef _DEBUG
		VulkanContext::init(VK_API_VERSION_1_0, false, false, vulkanInstanceExtensions);
#else
		VulkanContext::init(VK_API_VERSION_1_0, true, true, vulkanInstanceExtensions);
#endif
	}

	uint32_t Context::createEnvironment()
	{
		m_environments.push_back({});
		return m_environments.back().getID();
	}

	Environment& Context::getEnvironment(const uint32_t id)
	{
		for (auto& environment : m_environments)
		{
			if (environment.getID() == id)
			{
				return environment;
			}
		}

		throw std::runtime_error("Environment with id " + std::to_string(id) + " not found");
	}

	void Context::destroyEnvironment(const uint32_t id)
	{
		for (auto it = m_environments.begin(); it != m_environments.end(); ++it)
		{
			if (it->getID() == id)
			{
				it->destroy();
				m_environments.erase(it);
				break;
			}
		}
	}

	void Context::destroyEnvironment(const Environment& environment)
	{
		destroyEnvironment(environment.getID());
	}

	Project& Context::loadProject(const std::string& path, const uint32_t gpuOverride)
	{
		Environment& env = getEnvironment(createEnvironment());
		const uint32_t proj = env.loadProject(path);
		env.build(gpuOverride);
		return env.getProject(proj);
	}

	VkInstance Context::getVulkanInstance()
	{
		return VulkanContext::getHandle();
	}

	void Context::destroy()
	{
		VulkanContext::free();
		m_environments.clear();
	}
} // namespace gflow
