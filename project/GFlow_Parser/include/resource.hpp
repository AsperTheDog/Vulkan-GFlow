#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils/logger.hpp"

#define EXPORT(type, name) Export<type> ##name{#name, this}
#define EXPORT_GROUP(name, title) Export<bool> _##name{title, this, true}
#define EXPORT_ENUM(name, context) Export<EnumExport> ##name{#name, this, context}

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
        ENUM,
        RESOURCE
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

        const T& operator*() const { return m_data; }
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
            union
            {
                EnumContext* enumContext = nullptr;
                Resource* (*resourceFactory)(const std::string&);
            };
        };

        struct ResourceData
        {
            uint32_t key;
            std::string type;
            bool isSubresource;
            std::unordered_map<std::string, std::string> data;
        };

        typedef std::unordered_map<uint32_t, ResourceData> ResourceEntries;


    public:
        Resource() { setID(0); }
        virtual ~Resource() = default;

        struct Ref { std::string path; };

        virtual std::string serialize();
        virtual void deserialize(const ResourceData& data, const ResourceEntries& dependencies);

        bool deserialize(std::string filename = "");

        virtual std::pair<std::string, std::string> get(const std::string& variable);
        virtual bool set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies);

        virtual void exportsChanged() {}
        virtual void exportChanged(const std::string& variable) {}
        [[nodiscard]] virtual std::vector<ExportData> getCustomExports() { return {}; }

        [[nodiscard]] std::vector<ExportData> getExports();

        [[nodiscard]] virtual std::string getType() const = 0;

        [[nodiscard]] std::string getPath() const { return m_path; }
        [[nodiscard]] uint32_t getID() const { return m_id; }

    protected:
        explicit Resource(const std::string_view path) : m_isSubresource(path.empty()), m_path(path) { setID(0); }

        bool m_isSubresource;
        std::string m_path;
        uint32_t m_id;

        std::vector<ExportData> m_exports;

        void registerExport(const ExportData& data) { m_exports.push_back(data); }

    private:
        void setID(uint32_t id = 0);

        inline static std::unordered_set<uint32_t> s_ids;

        template <typename T>
        friend class Export;

        friend class Project;
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
        else if constexpr (std::is_pointer_v<T> && std::is_base_of_v<Resource, std::remove_pointer_t<T>>)
        {
            data.type = RESOURCE;
            data.resourceFactory = std::remove_pointer_t<T>::create;
            data.data = &this->m_data;
        }
        else
        {
            data.type = NONE;
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

