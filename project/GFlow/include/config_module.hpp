#pragma once
#include <string>

#include "internal/serialization.hpp"
#include "utils/logger.hpp"

namespace gflow {

    class ConfigModule final : public Serializable
    {
    public:
        ConfigModule();

        void set(std::string_view key, std::string_view value) override;
        void serialize(std::string_view filename) const override;

        static ConfigModule load(std::string_view path);

        // Getters and setters
        [[nodiscard]] uint32_t gpu() const { return m_gpu.get(); }
        [[nodiscard]] LoggerLevels logLevel() const { return m_logLevel.get(); }
        [[nodiscard]] bool debug() const { return m_debug.get(); }

        void setGpu(const uint32_t gpu) { m_gpu.set(gpu); }
        void setLogLevel(const LoggerLevels logLevel) { m_logLevel.set(logLevel); }
        void setDebug(const bool debug) { m_debug.set(debug); }

    private:
        [[nodiscard]] std::string getSerialized(std::string_view key) const override;

        [[nodiscard]] std::vector<std::string> keys() const override;
        [[nodiscard]] bool isSubresource(std::string_view key) const override;
        Serializable* getSubresource(std::string_view key) override;

        void addSubresource(std::string_view key, Serialization::ResourceData& subresource) override;

        // Config values

        SerializeEntry<uint32_t> m_gpu{UINT32_MAX, "gpu", {}};
#ifndef _DEBUG
        SerializeEntry<Logger::LevelBits> m_logLevel{Logger::WARN | Logger::ERR, "logLevel", {}};
        SerializeEntry<bool> m_debug{false, "debug", {}};
#else
        SerializeEntry<LoggerLevels> m_logLevel{Logger::INFO | Logger::WARN | Logger::ERR, "logLevel", {}};
        SerializeEntry<bool> m_debug{true, "debug", {}};
#endif
        friend class Context;
    };
}

