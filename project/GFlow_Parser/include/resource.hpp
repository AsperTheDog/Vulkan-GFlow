#pragma once
#include <string>
#include <vector>

#define IF_CHECK_TYPE(var, type, expr) if constexpr (std::is_same_v<var, type>) { expr; }
#define ELIF_CHECK_TYPE(var, type, expr) else IF_CHECK_TYPE(var, type, expr)

#define EXPORT(type, name) Export<type> ##name{#name, this}
#define EXPORT_GROUP(var, name) Export<bool> _##var{name, this, true}

namespace gflow::parser
{
    class Project;
    class Resource;

    enum DataType
    {
        STRING,
        INT,
        FLOAT,
        BOOL,
        REF
    };

    template <typename T>
    class Export
    {
    public:
        Export(std::string_view name, Resource* parent, bool group = false);

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
            std::string_view name;
            void* data = nullptr;
        };

    public:
        virtual ~Resource() = default;

        struct Ref { std::string path; };

        virtual std::string serialize();

        virtual std::string get(std::string_view variable);
        virtual void set(std::string_view variable, std::string_view value);

        virtual std::vector<std::string_view> getCustomExports() { return {}; }
        virtual std::vector<std::string_view> getDependencies();

        [[nodiscard]] const std::vector<ExportData>& getExports() const { return m_exports; }

        [[nodiscard]] virtual std::string getType() const = 0;

        [[nodiscard]] std::string_view getPath() const { return m_path; }
        [[nodiscard]] uint32_t getID() const { return m_id; }

        [[nodiscard]] static DataType getTokenType(const std::string& token);

    protected:
        explicit Resource(const std::string_view path, Project* project) : m_path(path), m_parent(project) {}

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
        IF_CHECK_TYPE(T, std::string, data.type = STRING)
        ELIF_CHECK_TYPE(T, int, data.type = INT)
        ELIF_CHECK_TYPE(T, float, data.type = FLOAT)
        ELIF_CHECK_TYPE(T, bool, data.type = BOOL)
        ELIF_CHECK_TYPE(T, Resource::Ref, data.type = REF)
        parent->registerExport(data);
    }
}

