#pragma once
#include <string>

#include "internal/serialization.hpp"
#include "utils/logger.hpp"

namespace gflow {

    class ConfigModule final : Serializable
    {
    public:
        ConfigModule();

        [[nodiscard]] std::string getSerialized(std::string_view key) const override;
        void set(std::string_view key, std::string_view value) override;

        [[nodiscard]] std::vector<std::string> keys() const override;
        [[nodiscard]] bool isSubresource(std::string_view key) const override;
        Serializable* getSubresource(std::string_view key) override;

        static ConfigModule load(std::string_view path);

        // Parameters

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

