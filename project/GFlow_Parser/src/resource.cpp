#include "resource.hpp"

#include <filesystem>
#include <fstream>

#include "resources/project.hpp"
#include "resource_manager.hpp"
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

    void Resource::initContext(ExportData* metadata)
    {
        if (!deserialize())
            serialize();
    }

    std::string Resource::serialize()
    {
        std::ofstream file;
        if (!m_isSubresource)
        {
            file.open(ResourceManager::getWorkingDir() + m_path);
            if (!file.is_open())
            {
                Logger::print("Failed to open file " + ResourceManager::getWorkingDir() + m_path, Logger::ERR);
                return "";
            }
        }
        std::string str;
        std::string dependencies;
        str += "[id=" + std::to_string(m_id) + ", type=" + getType() + ", level=" + (m_isSubresource ? "Subresource" : "Main") + "]\n";
        for (const ExportData& exportData : getExports())
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
        std::ifstream file(ResourceManager::getWorkingDir() + filename);
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
        for (const ExportData& exportData : getExports())
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
            case VEC2:
                return { std::to_string(static_cast<Vec2*>(exportData.data)->x) + ", " + std::to_string(static_cast<Vec2*>(exportData.data)->y), "" };
            case VEC3:
                return { std::to_string(static_cast<Vec3*>(exportData.data)->x) + ", " + std::to_string(static_cast<Vec3*>(exportData.data)->y) + ", " + std::to_string(static_cast<Vec3*>(exportData.data)->z), "" };
            case VEC4:
                return { std::to_string(static_cast<Vec4*>(exportData.data)->x) + ", " + std::to_string(static_cast<Vec4*>(exportData.data)->y) + ", " + std::to_string(static_cast<Vec4*>(exportData.data)->z) + ", " + std::to_string(static_cast<Vec4*>(exportData.data)->w), "" };
            case ENUM_BITMASK:
            case ENUM:
                return { std::to_string(static_cast<EnumExport*>(exportData.data)->id), "" };
            case RESOURCE:
            {
                Resource** resource = static_cast<Resource**>(exportData.data);
                if (*resource == nullptr)
                    return { "null", "" };
                if (!(*resource)->m_isSubresource)
                    return { (*resource)->getPath(), "" };
                return { std::to_string((*resource)->getID()),(*resource)->serialize() };
            }
            case NONE:
                Logger::print("Export type not supported", Logger::ERR);
            }
        }
        return { "", "" };
    }

    bool Resource::set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies)
    {
        for (ExportData& exportData : getExports())
        {
            if (variable != exportData.name) continue;
            switch (exportData.type)
            {
            case STRING:
                *static_cast<std::string*>(exportData.data) = value;
                return true;
            case INT:
                *static_cast<int*>(exportData.data) = std::stoi(value);
                return true;
            case FLOAT:
                *static_cast<float*>(exportData.data) = std::stof(value);
                return true;
            case BOOL:
                *static_cast<bool*>(exportData.data) = value != "0";
                return true;
            case VEC2:
            {
                Vec2* vec = static_cast<Vec2*>(exportData.data);
                const std::vector<std::string> values = string::split(value, ", ");
                if (values.size() != 2)
                {
                    Logger::print("Invalid value for Vec2: " + value, Logger::ERR);
                    return false;
                }
                vec->x = std::stof(values[0]);
                vec->y = std::stof(values[1]);
                return true;
            }
            case VEC3:
            {
                Vec3* vec = static_cast<Vec3*>(exportData.data);
                const std::vector<std::string> values = string::split(value, ", ");
                if (values.size() != 3)
                {
                    Logger::print("Invalid value for Vec3: " + value, Logger::ERR);
                    return false;
                }
                vec->x = std::stof(values[0]);
                vec->y = std::stof(values[1]);
                vec->z = std::stof(values[2]);
                return true;
            }
            case VEC4:
            {
                Vec4* vec = static_cast<Vec4*>(exportData.data);
                const std::vector<std::string> values = string::split(value, ", ");
                if (values.size() != 4)
                {
                    Logger::print("Invalid value for Vec3: " + value, Logger::ERR);
                    return false;
                }
                vec->x = std::stof(values[0]);
                vec->y = std::stof(values[1]);
                vec->z = std::stof(values[2]);
                vec->w = std::stof(values[3]);
                return true;
            }
            case ENUM_BITMASK:
            case ENUM:
                static_cast<EnumExport*>(exportData.data)->id = std::stoi(value);
                return true;
            case RESOURCE:
            {
                if (value == "null")
                {
                    *static_cast<Resource**>(exportData.data) = nullptr;
                    break;
                }
                try
                {
                    const uint32_t id = std::stoi(value);
                    if (id == m_id)
                    {
                        Logger::print("Resource with id " + std::to_string(id) + " cannot be a dependency of itself", Logger::ERR);
                        return false;
                    }
                    if (dependencies.contains(id))
                    {
                        *static_cast<Resource**>(exportData.data) = exportData.resourceFactory("", &exportData);
                        (*static_cast<Resource**>(exportData.data))->deserialize(dependencies.at(id), dependencies);
                        return true;
                    }
                    Logger::print("Resource with id " + std::to_string(id) + " not found in dependencies", Logger::ERR);
                }
                catch (const std::invalid_argument&)
                {
                    Resource& res = ResourceManager::loadResource(value);
                    *static_cast<Resource**>(exportData.data) = &res;
                    return true;
                }
            }
            default:
                Logger::print("Export type not supported for export " + variable, Logger::ERR);
                return false;
            }
        }
        return false;
    }

    std::vector<Resource::ExportData> Resource::getExports()
    {
        std::vector<ExportData> exports = m_exports;
        std::vector<ExportData> custom = getCustomExports();
        if (!custom.empty())
            exports.insert(exports.end(), custom.begin(), custom.end());
        return exports;
    }

    void Resource::setID(const uint32_t id)
    {
        m_id = id;
        while (m_id == 0 || s_ids.contains(m_id))
            m_id = rand();
        s_ids.insert(m_id);
    }
}
