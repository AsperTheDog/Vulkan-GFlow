#include <fstream>
#include <string>
#include <unordered_set>

#include "internal/serialization.hpp"

namespace gflow
{
    inline Serializable::Serializable(const std::string_view type, const std::string_view suffix)
        : m_type(type), m_suffix(suffix)
    {

    }

    inline const Serializable* Serializable::getSubresource(const std::string_view key) const
    {
        return const_cast<Serializable*>(this)->getSubresource(key);
    }

    void Serializable::setID(const uint32_t id)
    {
        m_id = id;
    }

    inline void Serialization::serialize(const Serializable& serializable, const std::string_view filename)
    {
        std::unordered_set<std::string> subresources;

        std::ofstream file(filename.data());
        if (!file.is_open()) throw std::runtime_error(std::string("Failed to open file + ") + filename.data() + " + for serialization");
        const std::string serialized = serializeEntry(serializable, true, subresources);
        file << serialized;
        file.close();
    }

    inline void Serialization::deserialize(Serializable& serializable, const std::string_view filename)
    {
        std::ifstream file(filename.data());
        if (!file.is_open()) throw std::runtime_error(std::string("Failed to open file + ") + filename.data() + " + for deserialization");
        std::unordered_map<std::string, std::string> subresources;
        for (std::string line; std::getline(file, line);)
        {
            
        }
    }

    inline std::string Serialization::serializeEntry(const Serializable& serializable, const bool isSubresource, std::unordered_set<std::string>& subresources)
    {
        std::string resource;
        resource += std::string("[") + (isSubresource ? "Subresource" : "Resource") + ", id = " + std::to_string(serializable.getId()) + ", type = " + serializable.m_type + "]\n";
        for (const auto& key : serializable.keys())
        {
            resource += key + " = " + serializable.getSerialized(key) + "\n";
            if (serializable.isSubresource(key) && !subresources.contains(key))
            {
                const Serializable* subresource = serializable.getSubresource(key);
                if (!subresource) throw std::runtime_error("Subresource " + key + " is null in " + serializable.m_type);
                subresources.insert(key);
                resource += serializeEntry(*subresource, false, subresources);
            }
        }
        return resource + "\n";
    }
}