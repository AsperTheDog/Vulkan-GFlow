#include "config_module.hpp"

#include <stdexcept>

ConfigModule::ConfigModule() : Serializable("cfg")
{
    
}

ConfigModule::ConfigModule(const std::string_view path) : ConfigModule()
{
    Serialization::deserialize(*this, path);
}

std::string ConfigModule::getSerialized(const std::string_view key) const
{
    if (m_gpu.doesNameMatch(key))
        return Serialization::serializePrimitive(m_gpu.get());
    if (m_logLevel.doesNameMatch(key))
        return Serialization::serializePrimitive(m_logLevel.get());
    if (m_debug.doesNameMatch(key))
        return Serialization::serializePrimitive(m_debug.get());

    throw std::runtime_error(std::string("Invalid key ") + key.data() + " in get for serializable of type " + m_suffix);
}

void ConfigModule::set(const std::string_view key, const std::string_view value)
{
    if (m_gpu.doesNameMatch(key))
        m_gpu.set(Serialization::deserializePrimitive<decltype(m_gpu.get())>(value.data()));
    if (m_debug.doesNameMatch(key))
        m_debug.set(Serialization::deserializePrimitive<decltype(m_debug.get())>(value.data()));
    if (m_logLevel.doesNameMatch(key))
    {
        m_logLevel.set(Serialization::deserializePrimitive<decltype(m_logLevel.get())>(value.data()));
        Logger::setLevels(m_logLevel.get());
    }
}

void ConfigModule::serialize(const std::string_view filename) const
{
    Serialization::serialize(*this, filename);
}

bool ConfigModule::isSubresource(std::string_view key) const
{
    return false;
}

Serializable* ConfigModule::getSubresource(const std::string_view key, const bool willEdit)
{
    return nullptr;
}

ConfigModule ConfigModule::load(const std::string_view path)
{
    ConfigModule config{};
    Serialization::deserialize(config, path);
    return config;
}

