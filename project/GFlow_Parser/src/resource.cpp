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
        for (const std::string_view& dep : getCustomExports())
            ss << dep << " = " << get(dep) << "\n";
        return ss.str();
    }

    std::string Resource::get(const std::string_view variable)
    {
        for (const ExportData& exportData : m_exports)
        {
            if (variable != exportData.name) continue;
            switch (exportData.type)
            {
            case STRING:
                return "'" + *static_cast<std::string*>(exportData.data) + "'";
            case INT:
                return std::to_string(*static_cast<int*>(exportData.data));
            case FLOAT:
                return std::to_string(*static_cast<float*>(exportData.data));
            case BOOL:
                return *static_cast<bool*>(exportData.data) ? "true" : "false";
            case REF:
                return static_cast<Resource::Ref*>(exportData.data)->path;
            case ENUM:
                return "E" + std::to_string(*static_cast<uint32_t*>(exportData.data));
            }
        }
        return "";
    }

    void Resource::set(std::string_view variable, const std::string_view value)
    {
        for (const ExportData& exportData : m_exports)
        {
            switch (exportData.type)
            {
            case STRING:
                *static_cast<std::string*>(exportData.data) = string::reduce(value.data());
                break;
            case INT:
                *static_cast<int*>(exportData.data) = std::stoi(value.data());
                break;
            case FLOAT:
                *static_cast<float*>(exportData.data) = std::stof(value.data());
                break;
            case BOOL:
                *static_cast<bool*>(exportData.data) = value == "true";
                break;
            case REF:
                static_cast<Resource::Ref*>(exportData.data)->path = string::reduce(value.data(), 1, 0);
                break;
            case ENUM:
                *static_cast<uint32_t*>(exportData.data) = std::stoi(value.data());
                break;
            }
        }
    }

    std::vector<std::string_view> Resource::getDependencies()
    {
        std::vector<std::string_view> dependencies;
        for (const ExportData& exportData : m_exports)
        {
            if (exportData.type == REF)
            {
                dependencies.push_back(static_cast<Resource::Ref*>(exportData.data)->path);
            }
        }
        return dependencies;
    }

    DataType Resource::getTokenType(const std::string& token)
    {
        if (token[0] == '\'')
            return STRING;
        if (token[0] == '"')
            return STRING;
        if (token[0] == '$')
            return REF;
        if (string::toLower(token) == "true" || string::toLower(token) == "false")
            return BOOL;
        if (token.find('.') != std::string::npos)
            return FLOAT;
        if (token[0] == 'E')
            return ENUM;
        return INT;
    }
}
