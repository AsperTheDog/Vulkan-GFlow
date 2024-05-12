#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/logger.hpp"

#define EXPORT(type, name) Export<type> ##name{#name, this}
#define EXPORT_LIST(type, name) Export<std::vector<(type)>> ##name{#name, this}
#define EXPORT_GROUP(name, title) Export<bool> _##name{title, this, true}
#define EXPORT_ENUM(name, context) Export<EnumExport> ##name{#name, this, context}
#define EXPORT_ENUM_LIST(name, context) Export<std::vector<EnumExport>> ##name{#name, this, context}

namespace gflow::parser
{
    class Project;
    class Resource;

    enum DataType
    {
        NONE,
        STRING,
        INT,
        FLOAT,
        BOOL,
        REF,
        ENUM,
        LIST_STRING,
        LIST_INT,
        LIST_FLOAT,
        LIST_BOOL,
        LIST_REF,
        LIST_ENUM,
    };

    struct EnumExport { uint32_t id; };
    struct EnumContext
    {
        std::vector<const char*> names;
        std::vector<uint32_t> values;
    };

    template <typename T>
    class Export
    {
    public:
        Export(std::string_view name, Resource* parent, bool group = false);
        Export(std::string_view name, Resource* parent, EnumContext& enumContext);

        T& operator*() { return m_data; }

    private:
        bool m_isGroup = false;
        std::string m_name;
        T m_data;
    };

    class Resource
    {
    public:
        struct ExportData
        {
            DataType type;
            std::string name;
            void* data = nullptr;
            EnumContext* enumContext = nullptr;
        };

    public:
        virtual ~Resource() = default;

        struct Ref { std::string path; };

        virtual std::string serialize();

        virtual std::string get(const std::string& variable);
        virtual void set(const std::string& variable, const std::string& value);

        virtual std::vector<std::string> getCustomExports() { return {}; }
        virtual std::vector<std::string> getDependencies();

        [[nodiscard]] const std::vector<ExportData>& getExports() const { return m_exports; }

        [[nodiscard]] virtual std::string getType() const = 0;

        [[nodiscard]] std::string getPath() const { return m_path; }
        [[nodiscard]] uint32_t getID() const { return m_id; }

    protected:
        explicit Resource(const std::string_view path, Project* project) : m_path(path), m_parent(project) {}

        [[nodiscard]] std::string getList(const ExportData& exportData) const;

        std::string m_path;
        uint32_t m_id = ++s_idCounter;
        Project* m_parent;

        std::vector<ExportData> m_exports;

        void registerExport(const ExportData& data) { m_exports.push_back(data); }

    private:
        inline static uint32_t s_idCounter = 0;

        template <typename T>
        friend class Export;
    };

    //***************************************************************
    //*********************** Implementations ***********************
    //***************************************************************

    template <typename T>
    Export<T>::Export(std::string_view name, Resource* parent, const bool group)
    {
        m_isGroup = group;
        Resource::ExportData data;
        this->m_name = name;
        data.name = name;

        if (m_isGroup)
        {
            parent->registerExport(data);
            return;
        }

        data.data = &this->m_data;
        if constexpr (std::is_same_v<T, std::string>) data.type = STRING;
        else if constexpr (std::is_same_v<T, int>) data.type = INT;
        else if constexpr (std::is_same_v<T, float>) data.type = FLOAT;
        else if constexpr (std::is_same_v<T, bool>) data.type = BOOL;
        else if constexpr (std::is_same_v<T, Resource::Ref>) data.type = REF;
        else if constexpr (std::is_same_v<T, Resource::Ref>) data.type = REF;
        else if constexpr (std::is_same_v<T, std::vector<EnumExport>>) data.type = ENUM;
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) data.type = LIST_STRING;
        else if constexpr (std::is_same_v<T, std::vector<int>>) data.type = LIST_INT;
        else if constexpr (std::is_same_v<T, std::vector<float>>) data.type = LIST_FLOAT;
        else if constexpr (std::is_same_v<T, std::vector<bool>>) data.type = LIST_BOOL;
        else if constexpr (std::is_same_v<T, std::vector<Resource::Ref>>) data.type = LIST_REF;
        else data.type = NONE;

        if (data.type == NONE)
        {
            Logger::print("Export type not supported", Logger::ERR);
            return;
        }

        parent->registerExport(data);
    }

    template <typename T>
    Export<T>::Export(std::string_view name, Resource* parent, EnumContext& enumContext)
    {
        Resource::ExportData data;
        this->m_name = name;
        data.name = name;
        data.enumContext = &enumContext;
        data.data = &this->m_data;
        data.type = ENUM;
        parent->registerExport(data);
    }
}

