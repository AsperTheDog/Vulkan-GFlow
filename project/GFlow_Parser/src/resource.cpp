#include "resource.hpp"

#include <filesystem>
#include <fstream>

#include "project.hpp"
#include "string_helper.hpp"

namespace gflow::parser
{
    static Resource::ResourceData parseToData(const std::string& serialized)
    {
        Resource::ResourceData data{};
        for (std::string& line : string::split(serialized, "\n"))
        {
            if (line.empty()) continue;

            // Header
            if (line[0] == '[')
            {
                std::erase_if(line, isspace);
                line = string::reduce(line);
                for (const std::string& part : string::split(line, ","))
                {
                    const auto [key, value] = string::tokenize(part, "=");
                    if (key == "id") data.key = std::stoi(value);
                    else if (key == "type") data.type = value;
                    else if (key == "level") data.isSubresource = value == "Subresource";
                }
                continue;
            }

            // Key-value pair
            const auto [key, value] = string::tokenize(line, "=");
            if (data.data.contains(key))
                Logger::print("Duplicate key " + key + " in " + data.type + " " + std::to_string(data.key) + " of type " + data.type, Logger::WARN);
            data.data[key] = value;
        }
        return data;
    }

    std::string Resource::serialize()
    {
        std::ofstream file;
        if (!m_isSubresource)
        {
            file.open(m_parent->getWorkingDir() + m_path);
            if (!file.is_open())
            {
                Logger::print("Failed to open file " + m_parent->getWorkingDir() + m_path, Logger::ERR);
                return "";
            }
        }
        std::string str;
        std::string dependencies;
        str += "[id=" + std::to_string(m_id) + ", type=" + getType() + ", level=" + (m_isSubresource ? "Subresource" : "Main") + "]\n";
        for (const ExportData& exportData : m_exports)
        {
            if (exportData.data == nullptr) continue;
            auto [value, subresources] = get(exportData.name);
            if (!subresources.empty())
                dependencies += subresources + "\n";
            str += exportData.name + " = " + value + "\n";
        }
        if (!dependencies.empty()) str += "\n" + dependencies;
        if (!m_isSubresource)
        {
            file << str;
            file.close();
        }
        return str;
    }

    bool Resource::deserialize(std::string filename)
    {
        if (filename.empty()) filename = m_path;
        ResourceEntries dependencies;
        ResourceData mainResource;
        std::ifstream file(m_parent->getWorkingDir() + filename);
        if (!file.is_open()) return false;
        std::string entry;
        std::string line;
        while (true)
        {
            std::getline(file, line);
            if (string::contains(line, "[") && !entry.empty())
            {
                ResourceData data = parseToData(entry);
                if (data.isSubresource) dependencies[data.key] = data;
                else mainResource = data;
                entry.clear();
            }
            if (!line.empty()) entry += line + "\n";
            if (file.eof()) break;
        }
        if (!entry.empty())
        {
            ResourceData data = parseToData(entry);
            if (data.isSubresource) dependencies[data.key] = data;
            else mainResource = data;
        }
        deserialize(mainResource, dependencies);
        return true;
    }

    void Resource::deserialize(const ResourceData& data, const ResourceEntries& dependencies)
    {
        for (const auto& [key, value] : data.data)
        {
            set(key, value, dependencies);
        }
    }

    std::pair<std::string, std::string> Resource::get(const std::string& variable)
    {
        for (const ExportData& exportData : m_exports)
        {
            if (variable != exportData.name) continue;
            switch (exportData.type)
            {
            case STRING:
                return { *static_cast<std::string*>(exportData.data), "" };
            case INT:
                return { std::to_string(*static_cast<int*>(exportData.data)), "" };
            case FLOAT:
                return { std::to_string(*static_cast<float*>(exportData.data)), "" };
            case BOOL:
                return { std::to_string(*static_cast<bool*>(exportData.data)), "" };
            case ENUM:
                return { std::to_string(static_cast<EnumExport*>(exportData.data)->id), "" };
            case RESOURCE:
                return { std::to_string(static_cast<Resource*>(exportData.data)->getID()), static_cast<Resource*>(exportData.data)->serialize() };
            case NONE:
                Logger::print("Export type not supported", Logger::ERR);
            }
        }
        return { "", "" };
    }

    void Resource::set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies)
    {
        for (const ExportData& exportData : m_exports)
        {
            if (variable != exportData.name) continue;
            switch (exportData.type)
            {
            case STRING:
                *static_cast<std::string*>(exportData.data) = value;
                break;
            case INT:
                *static_cast<int*>(exportData.data) = std::stoi(value);
                break;
            case FLOAT:
                *static_cast<float*>(exportData.data) = std::stof(value);
                break;
            case BOOL:
                *static_cast<bool*>(exportData.data) = value == "true";
                break;
            case ENUM:
                static_cast<EnumExport*>(exportData.data)->id = std::stoi(value);
                break;
            case RESOURCE:
            {
                const uint32_t id = std::stoi(value);
                if (id == m_id)
                {
                    Logger::print("Resource with id " + std::to_string(id) + " cannot be a dependency of itself", Logger::ERR);
                    return;
                }
                if (dependencies.contains(id))
                    static_cast<Resource*>(exportData.data)->deserialize(dependencies.at(id), dependencies);
                else
                    Logger::print("Resource with id " + std::to_string(id) + " not found in dependencies", Logger::ERR);
                break;
            }
            default:
                Logger::print("Export type not supported for export " + variable, Logger::ERR);
                return;
            }
        }
    }
}
