#include "config_module.hpp"

#include <stdexcept>

namespace gflow
{
    ConfigModule::ConfigModule() : Serializable("cfg")
    {
        
    }

    std::string ConfigModule::getSerialized(const std::string_view key) const
    {
        if (m_gpu.doesNameMatch(key) != MatchType::INVALID)
            return Serialization::serializePrimitive(m_gpu.get());
        if (m_logLevel.doesNameMatch(key) != MatchType::INVALID)
            return Serialization::serializePrimitive(m_logLevel.get());
        if (m_debug.doesNameMatch(key) != MatchType::INVALID)
            return Serialization::serializePrimitive(m_debug.get());

        throw std::runtime_error(std::string("Invalid key ") + key.data() + " in get for serializable of type " + m_suffix);
    }

    void ConfigModule::set(const std::string_view key, const std::string_view value)
    {
        if (m_gpu.doesNameMatch(key) != MatchType::INVALID)
            m_gpu.set(Serialization::deserializePrimitive<uint32_t>(value.data()));
        if (m_debug.doesNameMatch(key) != MatchType::INVALID)
            m_debug.set(Serialization::deserializePrimitive<bool>(value.data()));
        if (m_logLevel.doesNameMatch(key) != MatchType::INVALID)
        {
            m_logLevel.set(Serialization::deserializePrimitive<LoggerLevels>(value.data()));
            Logger::setLevels(m_logLevel.get());
        }
    }

    void ConfigModule::serialize(const std::string_view filename) const
    {
        Serialization::serialize(*this, filename);
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

    void ConfigModule::addSubresource(std::string_view key, Serialization::ResourceData& subresource)
    {

    }

    ConfigModule ConfigModule::load(const std::string_view path)
    {
        ConfigModule config{};
        Serialization::deserialize(config, path);
        return config;
    }
}
