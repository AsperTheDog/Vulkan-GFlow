#pragma once
#include "config_module.hpp"
#include "internal/serialization.hpp"
#include "internal/serialized_list.hpp"

namespace gflow
{
    class Project final : Serializable
    {
    public:
        Project();
        explicit Project(std::string_view filename);

        void set(std::string_view key, std::string_view value) override;
        void serialize(std::string_view filename) const override;

        static Project load(std::string_view path);

    protected:
        [[nodiscard]] std::string getSerialized(std::string_view key) const override;

        [[nodiscard]] bool isSubresource(std::string_view key) const override;
        Serializable* getSubresource(std::string_view key, bool willEdit = false) override;
    };
}

