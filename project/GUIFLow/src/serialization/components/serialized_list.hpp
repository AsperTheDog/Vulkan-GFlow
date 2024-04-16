#pragma once
#include <stdexcept>
#include <string>

#include "serialization/serialization.hpp"

template <typename T>
class SerializedList final : public Serializable
{
public:
    SerializedList();
    explicit SerializedList(std::string_view filename);

    void set(std::string_view key, std::string_view value) override;
    void serialize(std::string_view filename) const override;

    static SerializedList load(std::string_view filename);

    // Vector functions
    void push_back(const T& element) { m_elements.getRef().push_back(element); }
    void push_back(T&& element) { m_elements.getRef().push_back(std::move(element)); }

    T pop_back() { T element = m_elements.getRef().back(); m_elements.getRef().pop_back(); return element; }
    T& at(size_t index) { return m_elements.getRef().at(index); }
    T& operator[](const size_t index) { return at(index); }
    [[nodiscard]] const T& at(size_t index) const { return m_elements.getRef().at(index); }
    [[nodiscard]] const T& operator[](const size_t index) const { return at(index); }
    [[nodiscard]] size_t size() const { return m_elements.getRef().size(); }

    void resize(size_t size) { m_elements.getRef().resize(size); }

protected:
    [[nodiscard]] std::string getSerialized(std::string_view key) const override;
    [[nodiscard]] std::vector<std::string> keys() const override;

    [[nodiscard]] bool isSubresource(std::string_view key) const override;
    Serializable* getSubresource(std::string_view key, bool willEdit = false) override;

private:
    SerializeEntry<std::vector<T>> m_elements{{}, "elements"};

    static_assert(std::is_fundamental_v<T> || std::is_enum_v<T> || std::is_base_of_v<Serializable, T>, "Template parameter must be a primitive type or inherit from serializable");
    static_assert(!std::is_same_v<T, void> && !std::is_same_v<T, std::nullptr_t> && !std::is_pointer_v<T>, "void, nullptr, and pointers are not serializable");
};

template <typename T>
SerializedList<T>::SerializedList() : Serializable("lst")
{
}

template <typename T>
SerializedList<T>::SerializedList(const std::string_view filename) : Serializable("lst")
{
    Serialization::deserialize(*this, filename);
}

template <typename T>
void SerializedList<T>::set(const std::string_view key, const std::string_view value)
{
    if (key == "size") {
        resize(std::stoi(std::string(value)));
    }
    else if constexpr (std::is_fundamental_v<T> || std::is_enum_v<T>)
    {
        if (key.starts_with(m_elements.getKey() + "/")) {
            const std::string index = std::string(key.substr(m_elements.getKey().size() + 1));
            const uint32_t i = std::stoi(index);
            if (i >= size()) resize(i + 1);
            at(i) = Serialization::deserializePrimitive<T>(std::string(value));
        }
    }
}

template <typename T>
void SerializedList<T>::serialize(const std::string_view filename) const
{
    Serialization::serialize(*this, filename);
}

template <typename T>
SerializedList<T> SerializedList<T>::load(const std::string_view filename)
{
    SerializedList list;
    Serialization::deserialize(list, filename);
    return list;
}

template <typename T>
std::string SerializedList<T>::getSerialized(const std::string_view key) const
{
    if (key == "size") {
        return std::to_string(m_elements.getRef().size());
    }
    if constexpr (std::is_fundamental_v<T> || std::is_enum_v<T>)
    {
        if (key.starts_with(m_elements.getKey() + "/")) {
            const std::string index = std::string(key.substr(m_elements.getKey().size() + 1));
            return Serialization::serializePrimitive(at(std::stoi(index)));
        }
    }
    throw std::runtime_error(std::string("Invalid key ") + key.data() + " in get for serializable of type " + m_suffix);
}

template <typename T>
std::vector<std::string> SerializedList<T>::keys() const
{
    std::vector<std::string> keys;
    keys.emplace_back("size");
    for (size_t i = 0; i < size(); ++i) {
        keys.push_back(m_elements.getKey() + "/" + std::to_string(i));
    }
    return keys;
}

template <typename T>
bool SerializedList<T>::isSubresource(std::string_view key) const
{
    if constexpr (std::is_fundamental_v<T> || std::is_enum_v<T>)
    {
        return false;
    }
    else
    {
        return key.starts_with(m_elements.getKey() + "/");
    }
}

template <typename T>
Serializable* SerializedList<T>::getSubresource(const std::string_view key, const bool willEdit)
{
    if constexpr (std::is_base_of_v<Serializable, T>)
    {
        if (!isSubresource(key)) {
            return nullptr;
        }
        const std::string index = std::string(key.substr(m_elements.getKey().size() + 1));
        const uint32_t i = std::stoi(index);
        if (willEdit && i >= size()) resize(i + 1);
        return &at(i);
    }
    else
    {
        return nullptr;
    }
}

