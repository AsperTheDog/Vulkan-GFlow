#include "resource.hpp"

#include <sstream>

#include "string_helper.hpp"

namespace gflow::parser
{
    std::string Resource::serialize()
    {
        std::stringstream ss;
        ss << "[" << getType() << "]\n";
        for (const ExportData& exportData : m_exports)
            ss << exportData.name << " = " << get(exportData.name) << "\n";
        for (const std::string& dep : getCustomExports())
            ss << dep << " = " << get(dep) << "\n";
        return ss.str();
    }

    std::string Resource::get(const std::string& variable)
    {
        for (const ExportData& exportData : m_exports)
        {
            if (variable != exportData.name) continue;
            switch (exportData.type)
            {
            case STRING:
                return "\"" + *static_cast<std::string*>(exportData.data) + "\"";
            case INT:
                return std::to_string(*static_cast<int*>(exportData.data));
            case FLOAT:
                return std::to_string(*static_cast<float*>(exportData.data));
            case BOOL:
                return *static_cast<bool*>(exportData.data) ? "true" : "false";
            case REF:
                return "$" + static_cast<Resource::Ref*>(exportData.data)->path;
            case ENUM:
                return "E" + std::to_string(static_cast<EnumExport*>(exportData.data)->id);
            case LIST_STRING:
            case LIST_INT:
            case LIST_FLOAT:
            case LIST_BOOL:
            case LIST_REF:
            case LIST_ENUM:
                return getList(exportData);
            case NONE:
                Logger::print("Export type not supported", Logger::ERR);
                return "";
            }
        }
        return "";
    }

#define INVALID_EXPORT_LIST_SIZE_SET(list, index) if ((index) >= (list)->size())                                                                 \
                                        {                                                                                            \
                                            Logger::print("Index out of bounds for list export, resizing" + variable, Logger::WARN); \
                                            (list)->resize((index) + 1);                                                             \
                                        }                                                                                            \


    void Resource::set(const std::string& variable, const std::string& value)
    {
        if (string::contains(variable, "/"))
        {
            const std::vector<std::string> parts = string::split(variable, "/");
            uint32_t index;
            try
            {
                index = std::stoi(parts[1]);
            }
            catch (const std::exception& e)
            {
                Logger::print("Invalid index for list export " + variable, Logger::ERR);
                return;
            }
            for (const ExportData& exportData : m_exports)
            {
                if (parts[0] != exportData.name) continue;
                switch (exportData.type)
                {
                case LIST_STRING:
                {
                    std::vector<std::string>* list = static_cast<std::vector<std::string>*>(exportData.data);
                    INVALID_EXPORT_LIST_SIZE_SET(list, index)
                    list->at(index) = string::reduce(value);
                    break;
                }
                case LIST_INT:
                {
                    std::vector<int>* list = static_cast<std::vector<int>*>(exportData.data);
                    INVALID_EXPORT_LIST_SIZE_SET(list, index)
                    list->at(index) = std::stoi(value);
                    break;
                }
                case LIST_FLOAT:
                {
                    std::vector<float>* list = static_cast<std::vector<float>*>(exportData.data);
                    INVALID_EXPORT_LIST_SIZE_SET(list, index)
                    list->at(index) = std::stof(value);
                    break;
                }
                case LIST_BOOL:
                {
                    std::vector<bool>* list = static_cast<std::vector<bool>*>(exportData.data);
                    INVALID_EXPORT_LIST_SIZE_SET(list, index)
                    list->at(index) = value == "true";
                    break;
                }
                case LIST_REF:
                {
                    std::vector<Ref>* list = static_cast<std::vector<Ref>*>(exportData.data);
                    INVALID_EXPORT_LIST_SIZE_SET(list, index)
                    list->at(index).path = string::reduce(value, 1, 0);
                    break;
                }
                case LIST_ENUM:
                {
                    std::vector<EnumExport>* list = static_cast<std::vector<EnumExport>*>(exportData.data);
                    INVALID_EXPORT_LIST_SIZE_SET(list, index)
                    list->at(index).id = std::stoi(string::reduce(value, 1, 0));
                    break;
                }
                default:
                    Logger::print("Malformed variable key " + variable, Logger::ERR);
                    return;
                }
            }
        }
        for (const ExportData& exportData : m_exports)
        {
            if (variable != exportData.name) continue;
            switch (exportData.type)
            {
            case STRING:
                *static_cast<std::string*>(exportData.data) = string::reduce(value);
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
            case REF:
                static_cast<Resource::Ref*>(exportData.data)->path = string::reduce(value, 1, 0);
                break;
            case ENUM:
                static_cast<EnumExport*>(exportData.data)->id = std::stoi(string::reduce(value, 1, 0));
                break;
            default:
                Logger::print("Export type not supported for export " + variable, Logger::ERR);
                return;
            }
        }
    }

    std::vector<std::string> Resource::getDependencies()
    {
        std::vector<std::string> dependencies;
        for (const ExportData& exportData : m_exports)
        {
            if (exportData.type == REF)
            {
                dependencies.push_back(static_cast<Resource::Ref*>(exportData.data)->path);
            }
        }
        return dependencies;
    }

    std::string Resource::getList(const ExportData& exportData) const
    {
        std::stringstream ss;

        switch (exportData.type)
        {
        case LIST_STRING:
        {
            const std::vector<std::string>* list = static_cast<std::vector<std::string>*>(exportData.data);
            ss << exportData.name << " = S" << list->size() << "\n";
            for (uint32_t i = 0; i < list->size(); ++i)
                ss << exportData.name << "/" << i << " = \"" << list->at(i) << "\"\n";
            break;
        }
        case LIST_INT:
        {
            const std::vector<int>* list = static_cast<std::vector<int>*>(exportData.data);
            ss << exportData.name << " = S" << list->size() << "\n";
            for (uint32_t i = 0; i < list->size(); ++i)
                ss << exportData.name << "/" << i << " = " << list->at(i) << "\n";
            break;
        }
        case LIST_FLOAT:
        {
            const std::vector<float>* list = static_cast<std::vector<float>*>(exportData.data);
            ss << exportData.name << " = S" << list->size() << "\n";
            for (uint32_t i = 0; i < list->size(); ++i)
                ss << exportData.name << "/" << i << " = " << list->at(i) << "\n";
            break;
        }
        case LIST_BOOL:
        {
            std::vector<bool>* list = static_cast<std::vector<bool>*>(exportData.data);
            ss << exportData.name << " = S" << list->size() << "\n";
            for (uint32_t i = 0; i < list->size(); ++i)
                ss << exportData.name << "/" << i << " = " << (list->at(i) ? "true" : "false") << "\n";
            break;
        }
        case LIST_REF:
        {
            const std::vector<Ref>* list = static_cast<std::vector<Ref>*>(exportData.data);
            ss << exportData.name << " = S" << list->size() << "\n";
            for (uint32_t i = 0; i < list->size(); ++i)
                ss << exportData.name << "/" << i << " = $" << list->at(i).path << "\n";
            break;
        }
        case LIST_ENUM:
        {
            const std::vector<EnumExport>* list = static_cast<std::vector<EnumExport>*>(exportData.data);
            ss << exportData.name << " = S" << list->size() << "\n";
            for (uint32_t i = 0; i < list->size(); ++i)
                ss << exportData.name << "/" << i << " = E" << list->at(i).id << "\n";
            break;
        }
        default:
            Logger::print("Invalid export type sent to getList for export " + exportData.name, Logger::ERR);
            return "";
        }
        return string::reduce(ss.str(), 0, 1);
    }
}
