#pragma once
#include "internal/serialization.hpp"

namespace gflow
{
    class Project final : Serializable
    {
    public:
        Project();

        void set(std::string_view key, std::string_view value) override;
        void serialize(std::string_view filename) const override;

        static Project load(std::string_view path);

    protected:
        [[nodiscard]] std::string getSerialized(std::string_view key) const override;

        [[nodiscard]] std::vector<std::string> keys() const override;
        [[nodiscard]] bool isSubresource(std::string_view key) const override;
        Serializable* getSubresource(std::string_view key) override;

        void addSubresource(std::string_view key, Serialization::ResourceData& subresource) override;        
    };
}

