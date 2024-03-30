#pragma once
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace gflow
{
    enum MatchType
    {
        CORRECT,
        LEGACY,
        INVALID
    };

    template <typename T>
    class SerializeEntry
    {
    public:
        SerializeEntry(T value, std::string_view name, std::initializer_list<std::string> legacyNames = {});

        [[nodiscard]] T get() const;
        void set(T value);

        void addLegacyName(std::string_view name);
        [[nodiscard]] MatchType doesNameMatch(std::string_view name) const;

    private:
        std::string m_key;
        T m_value;
        std::vector<std::string> m_legacy;
    };

    class Serializable
    {
    public:
        [[nodiscard]] virtual std::string getSerialized(std::string_view key) const = 0;
        virtual void set(std::string_view key, std::string_view value) = 0;
        [[nodiscard]] virtual std::vector<std::string> keys() const = 0;

        [[nodiscard]] virtual bool isSubresource(std::string_view key) const = 0;
        [[nodiscard]] const Serializable* getSubresource(std::string_view key) const;
        virtual Serializable* getSubresource(std::string_view key) = 0;

        [[nodiscard]] uint32_t getId() const;
        
    protected:
        explicit Serializable(std::string_view suffix);
        ~Serializable() = default;

    private:
        void setID(uint32_t id);

        std::string m_suffix;

        uint32_t m_id = ++s_idCounter;

        inline static uint32_t s_idCounter = 0;

        friend class Serialization;
    };
    

    class Serialization
    {
    public:
        struct ResourceData
        {
            uint32_t key;
            std::string type;
            bool isSubresource;
            std::unordered_map<std::string, std::string> data;
        };

        static void serialize(const Serializable& serializable, std::string_view filename);
        static void deserialize(Serializable& serializable, std::string_view filename);

    private:
        static std::string serializeEntry(const Serializable& serializable, bool isSubresource, std::unordered_set<std::string>& subresources);

        static ResourceData deserializeEntry(const std::string& serialized, std::vector<ResourceData>& subresources);
        static void deserializeHeader(std::string serialized, ResourceData& data);
        static std::pair<std::string, std::string> deserializeKeyValue(const std::string& serialized);
    };

    // Inline definitions
    
    template <typename T>
    SerializeEntry<T>::SerializeEntry(T value, const std::string_view name, const std::initializer_list<std::string> legacyNames)
    {
        m_value = value;
        m_key = name;
        for (const auto& legacy : legacyNames)
        {
            m_legacy.emplace_back(legacy);
        }
    }

    template <typename T>
    T SerializeEntry<T>::get() const
    {
        return m_value;
    }

    template <typename T>
    void SerializeEntry<T>::set(T value)
    {
        m_value = value;
    }

    template <typename T>
    void SerializeEntry<T>::addLegacyName(std::string_view name)
    {
        m_legacy.emplace_back(name);
    }

    template <typename T>
    MatchType SerializeEntry<T>::doesNameMatch(const std::string_view name) const
    {
        if (name == m_key)
            return CORRECT;

        for (const auto& legacy : m_legacy)
        {
            if (name == legacy)
                return LEGACY;
        }

        return INVALID;
    }

} // namespace gflow

