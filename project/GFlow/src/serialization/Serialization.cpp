#include "internal/serialization.hpp"

#include <fstream>

#include "utils/logger.hpp"

namespace gflow
{
    Serializable::Serializable(const std::string_view suffix)
        : m_suffix(suffix)
    {

    }

    const Serializable* Serializable::getSubresource(const std::string_view key) const
    {
        return const_cast<Serializable*>(this)->getSubresource(key);
    }

    uint32_t Serializable::getId() const
    {
        return m_id;
    }

    void Serializable::setID(const uint32_t id)
    {
        m_id = id;
    }

    void Serialization::serialize(const Serializable& serializable, const std::string_view filename)
    {
        std::unordered_set<std::string> subresources;

        std::ofstream file(filename.data());
        if (!file.is_open()) throw std::runtime_error(std::string("Failed to open file + ") + filename.data() + " + for serialization");
        std::string serialized = serializeEntry(serializable, false, subresources);
        while (serialized.back() == '\n') serialized.pop_back();
        file << serialized;
        file.close();
    }

    void Serialization::deserialize(Serializable& serializable, const std::string_view filename)
    {
        std::ifstream file(filename.data());
        if (!file.is_open()) throw std::runtime_error(std::string("Failed to open file + ") + filename.data() + " + for deserialization");
        std::vector<ResourceData> subresources;
        std::string entry;
        std::string line;
        while (true)
        {
            std::getline(file, line);
            if (line.find('[') == std::string::npos || entry.empty())
            {
                if (!line.empty()) entry += line + '\n';
                if (!file.eof()) continue;
            }
            ResourceData data = deserializeEntry(entry, subresources);
            if (data.isSubresource)
            {
                subresources.push_back(data);
            }
            else
            {
                if (data.type != serializable.m_suffix)
                {
                    Logger::print(std::string("Type mismatch in ") + filename.data() + ": expected " + serializable.m_suffix + ", got " + data.type, Logger::WARN);
                }
                for (const auto& [key, value] : data.data)
                {
                    if (serializable.isSubresource(key))
                    {
                        uint32_t id;
                        try
                        {
                            id = std::stoi(value);
                            if (id >= subresources.size()) throw std::invalid_argument("");
                        }
                        catch (const std::invalid_argument&)
                        {
                            Logger::print("Invalid ID " + value + " for subresource " + key + " in " + serializable.m_suffix, Logger::WARN);
                            continue;
                        }
                        serializable.addSubresource(key, subresources.at(id));
                    }
                    else
                    {
                        serializable.set(key, value);
                    }
                }
            }
            if (!line.empty()) entry = line + "\n";
            if (file.eof()) break;
        }
    }

    std::string Serialization::serializeEntry(const Serializable& serializable, const bool isSubresource, std::unordered_set<std::string>& subresources)
    {
        std::string resource;
        resource += std::string("[level = ") + (isSubresource ? std::string("Subresource") + ", id = " + std::to_string(serializable.getId()) : "Resource") + ", type = " + serializable.m_suffix + "]\n";
        for (const auto& key : serializable.keys())
        {
            if (serializable.isSubresource(key) && !subresources.contains(key))
            {
                const Serializable* subresource = serializable.getSubresource(key);
                if (!subresource) throw std::runtime_error("Subresource " + key + " is null in " + serializable.m_suffix);
                subresources.insert(key);
                resource.insert(0, serializeEntry(*subresource, true, subresources));
                resource += key + " = " + std::to_string(subresource->getId()) + "\n";
            }
            else
            {
                resource += key + " = " + serializable.getSerialized(key) + "\n";
            }
        }
        return resource + "\n";
    }

    Serialization::ResourceData Serialization::deserializeEntry(const std::string& serialized, std::vector<ResourceData>& subresources)
    {
        ResourceData data;
        uint32_t lineCount = 0;
        for (uint32_t i = 0; i < serialized.size(); ++i)
        {
            if (serialized[i] == '\n')
            {
                if (lineCount == 0) continue;
                
                std::string line = serialized.substr(i - lineCount, lineCount);
                lineCount = 0;
                std::erase_if(line, isspace);

                if (line.empty()) continue;

                // Header
                if (line[0] == '[')
                {
                    deserializeHeader(line, data);
                    continue;
                }

                // Key-value pair
                const auto [key, value] = deserializeKeyValue(line);
                if (data.data.contains(key)) Logger::print("Duplicate key " + key + " in " + data.type + " " + std::to_string(data.key) + " of type " + data.type, Logger::WARN);
                data.data[key] = value;
            }
            ++lineCount;
        }
        return data;
    }

    void Serialization::deserializeHeader(std::string serialized, ResourceData& data)
    {
        //Strip brackets
        serialized.erase(0, 1);
        serialized.pop_back();

        for (size_t i = 0, j; i < serialized.size(); i = j + 1)
        {
            //Split by commas
            j = serialized.find(',', i);
            if (j == std::string::npos) j = serialized.size();
            std::string part = serialized.substr(i, j - i);

            //Parse key-value pairs
            const auto [key, value] = deserializeKeyValue(part);
            if (key == "id") data.key = std::stoi(value);
            else if (key == "type") data.type = value;
            else if (key == "level") data.isSubresource = value == "Subresource";
        }
    }

    std::pair<std::string, std::string> Serialization::deserializeKeyValue(const std::string& serialized)
    {
        std::string key = serialized.substr(0, serialized.find('='));
        std::string value = serialized.substr(serialized.find('=') + 1);
        return { key, value };
    }
}
