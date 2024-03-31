#pragma once
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace gflow
{
    class Serializable;

    enum MatchType
    {
        CORRECT = 1,
        LEGACY = 2,
        INVALID = 0
    };

    template <typename T>
    class SerializeEntry
    {
    public:
        SerializeEntry(Serializable* parent, T value, std::string_view name, std::initializer_list<std::string> legacyNames = {});
        SerializeEntry(T value, std::string_view name, std::initializer_list<std::string> legacyNames = {});

        [[nodiscard]] T get() const;
        T& getRef();
        const T& getRef() const;
        void set(T value);

        void addLegacyName(std::string_view name);
        [[nodiscard]] MatchType doesNameMatch(std::string_view name) const;

        [[nodiscard]] std::string getKey() const { return m_key; };

    private:
        std::string m_key;
        T m_value;
        std::vector<std::string> m_legacy;
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

        template <typename T>
        static T deserializePrimitive(std::string value);

        template <typename T>
        static std::string serializePrimitive(T value);

    private:
        static std::string serializeEntry(const Serializable& serializable, bool isSubresource, std::unordered_set<std::string>& subresources);

        static ResourceData parseToData(const std::string& serialized);
        static void deserializeEntry(Serializable& serializable, const ResourceData& serialized, std::unordered_map<uint32_t, ResourceData>& subresources);
        static void deserializeHeader(std::string serialized, ResourceData& data);
        static std::pair<std::string, std::string> deserializeKeyValue(const std::string& serialized);
    };

    class Serializable
    {
    public:
        virtual void set(std::string_view key, std::string_view value) = 0;

        [[nodiscard]] uint32_t getId() const;

        virtual void serialize(std::string_view filename) const = 0;

    protected:
        [[nodiscard]] virtual std::string getSerialized(std::string_view key) const = 0;
        [[nodiscard]] virtual std::vector<std::string> keys() const;

        [[nodiscard]] virtual bool isSubresource(std::string_view key) const = 0;
        [[nodiscard]] const Serializable* getSubresource(const std::string_view key) const;
        virtual Serializable* getSubresource(std::string_view key, bool willEdit = false) = 0;

        explicit Serializable(std::string_view suffix);
        ~Serializable() = default;

        std::string m_suffix;

    private:
        void setID(uint32_t id);

        uint32_t m_id = ++s_idCounter;
        std::vector<std::string> m_keys;

        inline static uint32_t s_idCounter = 0;

        friend class Serialization;
        template<typename T>
        friend class SerializeEntry;
    };

    // Inline definitions
    
    template <typename T>
    SerializeEntry<T>::SerializeEntry(Serializable* parent, T value, const std::string_view name, const std::initializer_list<std::string> legacyNames)
        : m_key(name), m_value(value)
    {
        if (parent != nullptr) parent->m_keys.emplace_back(name);
        for (const auto& legacy : legacyNames)
        {
            m_legacy.emplace_back(legacy);
        }
    }

    template <typename T>
    SerializeEntry<T>::SerializeEntry(T value, const std::string_view name, std::initializer_list<std::string> legacyNames)
        : SerializeEntry(nullptr, value, name, legacyNames)
    {

    }

    template <typename T>
    T SerializeEntry<T>::get() const
    {
        return m_value;
    }

    template <typename T>
    T& SerializeEntry<T>::getRef()
    {
        return m_value;
    }

    template <typename T>
    const T& SerializeEntry<T>::getRef() const
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

    template <typename T>
    T Serialization::deserializePrimitive(const std::string value)
    {
        if constexpr (std::is_same_v<T, int>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, short>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, long>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, long long>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, char>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, unsigned int>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, unsigned short>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, unsigned long>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, unsigned long long>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, unsigned char>) 
        {
            return std::stoi(value);
        }
        if constexpr (std::is_same_v<T, float>) 
        {
            return std::stof(value);
        }
        if constexpr (std::is_same_v<T, double>) 
        {
            return std::stod(value);
        }
        if constexpr (std::is_enum_v<T>) 
        {
            return static_cast<T>(std::stoi(value));
        }
        if constexpr (std::is_same_v<T, bool>) 
        {
            return value == "true";
        }

        throw std::runtime_error("Unsupported primitive type");
    }

    template <typename T>
    std::string Serialization::serializePrimitive(T value)
    {
        if constexpr (std::is_same_v<T, int>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, short>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, long>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, long long>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, char>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, unsigned int>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, unsigned short>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, unsigned long>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, unsigned long long>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, unsigned char>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, float>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_same_v<T, double>) 
        {
            return std::to_string(value);
        }
        if constexpr (std::is_enum_v<T>)
        {
            return std::to_string(static_cast<int>(value));
        }
        if constexpr (std::is_same_v<T, bool>) 
        {
            return value ? "true" : "false";
        }

        throw std::runtime_error("Unsupported primitive type");
    }
} // namespace gflow

