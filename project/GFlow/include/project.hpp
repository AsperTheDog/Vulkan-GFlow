#pragma once
#include "internal/serialization.hpp"

namespace gflow
{
    class Project final : Serializable
    {
    public:
        Project();

        [[nodiscard]] std::string getSerialized(std::string_view key) const override;
        void set(std::string_view key, std::string_view value) override;

        [[nodiscard]] std::vector<std::string> keys() const override;
        [[nodiscard]] bool isSubresource(std::string_view key) const override;
        Serializable* getSubresource(std::string_view key) override;

        static Project load(std::string_view path);

    private:
        
    };
}

