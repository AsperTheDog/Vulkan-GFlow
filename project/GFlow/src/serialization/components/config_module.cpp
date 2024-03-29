#include "config_module.hpp"

#include <stdexcept>

namespace gflow
{
    ConfigModule::ConfigModule() : Serializable("Config", "cfg")
    {
        
    }

    std::string ConfigModule::getSerialized(const std::string_view key) const
    {
        if (m_gpu.doesNameMatch(key) != MatchType::INVALID)
            return std::to_string(m_gpu.get());
        if (m_logLevel.doesNameMatch(key) != MatchType::INVALID)
        {
            if ((m_logLevel.get() & Logger::DEBUG) == 0)
                return "debug";
            if ((m_logLevel.get() & Logger::INFO) == 0)
                return "info";
            if ((m_logLevel.get() & Logger::WARN) == 0)
                return "warn";
            if ((m_logLevel.get() & Logger::ERR) == 0)
                return "error";
            return "none";
        }
        if (m_debug.doesNameMatch(key) != MatchType::INVALID)
            return m_debug.get() ? "true" : "false";

        throw std::runtime_error("Invalid key in get");
    }

    void ConfigModule::set(const std::string_view key, const std::string_view value)
    {
        if (m_gpu.doesNameMatch(key) != MatchType::INVALID)
            m_gpu.set(std::stoi(static_cast<std::string>(value)));
        if (m_logLevel.doesNameMatch(key) != MatchType::INVALID)
        {
            if (value == "debug")
                m_logLevel.set(Logger::ALL);
            else if (value == "info")
                m_logLevel.set(Logger::INFO | Logger::WARN | Logger::ERR);
            else if (value == "warn")
                m_logLevel.set(Logger::WARN | Logger::ERR);
            else if (value == "error")
                m_logLevel.set(Logger::ERR);
            else
                m_logLevel.set(Logger::NONE);
            Logger::setLevels(m_logLevel.get());
        }
        if (m_debug.doesNameMatch(key) != MatchType::INVALID)
            m_debug.set(value == "true");
    }

    std::vector<std::string> ConfigModule::keys() const
    {
        return {"gpu", "logLevel", "debug"};
    }

    bool ConfigModule::isSubresource(std::string_view key) const
    {
        return false;
    }

    Serializable* ConfigModule::getSubresource(std::string_view key)
    {
        return nullptr;
    }

    ConfigModule ConfigModule::load(const std::string_view path)
    {
        ConfigModule config{};
        Serialization::deserialize(config, path);
        return config;
    }
}
