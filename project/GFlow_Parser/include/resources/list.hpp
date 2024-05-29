#pragma once
#include "resource.hpp"

#define EXPORT_LIST(type, name) Export<List<type>*> ##name{#name, this}
#define EXPORT_ENUM_LIST(name, context) Export<List<EnumExport>*> ##name{#name, this, context}
#define EXPORT_RESOURCE_LIST(type, name) Export<List<type*>*> ##name{#name, this}

namespace gflow::parser
{
    // template cannot be bool
    template <typename T>
    class List final : public Resource
    {
    public:
        [[nodiscard]] EnumContext* getEnumContext() const { return m_enumContext; }

        [[nodiscard]] int size() const { return m_size; }
        [[nodiscard]] const T& operator[](int index) const { return m_data[index]; }
        [[nodiscard]] T& operator[](int index) { return m_data[index]; }

        void initContext(ExportData* metadata) override;

        void setEnumContext(EnumContext* enumContext) { m_enumContext = enumContext; }

        bool set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies) override;
        [[nodiscard]] std::vector<ExportData> getCustomExports() override;
        void exportsChanged() override;
        void exportChanged(const std::string& variable) override;

    private:
        int m_size = 0;
        std::vector<T> m_data;
        EnumContext* m_enumContext = nullptr;

        Resource* m_parent = nullptr;
        
    public:
        DECLARE_RESOURCE(List)

    private:
        static_assert(!std::is_same_v<T, bool>, "bool is not allowed as the template parameter for MyClass");
    };

    template <typename T>
    void List<T>::initContext(ExportData* metadata)
    {
        setEnumContext(metadata->enumContext);
    }

    template <typename T>
    bool List<T>::set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies)
    {
        if (variable == "size")
        {
            m_size = std::stoi(value);
            exportChanged("size");
            return true;
        }
        return Resource::set(variable, value, dependencies);
    }

    template <typename T>
    std::vector<Resource::ExportData> List<T>::getCustomExports()
    {
        std::vector<ExportData> exports;
        ExportData sizeData;
        sizeData.data = &m_size;
        sizeData.type = INT;
        sizeData.name = "size";
        exports.push_back(sizeData);

        for (int i = 0; i < m_size; i++)
        {
            ExportData data;
            data.data = &m_data[i];
            if constexpr (std::is_same_v<T, std::string>) data.type = STRING;
            else if constexpr (std::is_same_v<T, int>) data.type = INT;
            else if constexpr (std::is_same_v<T, float>) data.type = FLOAT;
            else if constexpr (std::is_same_v<T, bool>) data.type = BOOL;
            else if constexpr (std::is_same_v<T, EnumExport>)
            {
                data.type = ENUM;
                data.enumContext = getEnumContext();
            }
            else if constexpr (std::is_pointer_v<T> && std::is_base_of_v<Resource, std::remove_pointer_t<T>>)
            {
                data.type = RESOURCE;
                data.resourceFactory = &Resource::create<std::remove_pointer_t<T>>;
                data.getType = std::remove_pointer_t<T>::getTypeStatic;
            }
            data.name = std::to_string(i);
            exports.push_back(data);
        }

        return exports;
    }

    template <typename T>
    void List<T>::exportsChanged()
    {
        if (m_size < 0) m_size = 0;
        if (m_data.size() != static_cast<size_t>(m_size))
            m_data.resize(m_size);
    }

    template <typename T>
    void List<T>::exportChanged(const std::string& variable)
    {
        if (variable == "size")
        {
            if (m_size < 0) m_size = 0;
            if (m_data.size() != static_cast<size_t>(m_size))
                m_data.resize(m_size);
        }
    }

    template <typename T>
    class Export<List<T>*>
    {
    public:
        Export(std::string_view name, Resource* parent);
        Export(std::string_view name, Resource* parent, EnumContext& enumContext);

        const List<T>& operator*() const { return *m_data; }
        List<T>& operator*() { return *m_data; }

    private:
        std::string m_name;
        List<T>* m_data;

        Resource* m_parent;
    };

    template <typename T>
    Export<List<T>*>::Export(std::string_view name, Resource* parent) : m_parent(parent)
    {
        Resource::ExportData data;
        this->m_name = name;
        data.name = name;
        data.type = RESOURCE;
        data.resourceFactory = &Resource::create<List<T>>;
        data.getType = List<T>::getTypeStatic;
        data.data = &this->m_data;

        parent->registerExport(data);
    }

    template <typename T>
    Export<List<T>*>::Export(std::string_view name, Resource* parent, EnumContext& enumContext) : m_parent(parent)
    {
        Resource::ExportData data;
        this->m_name = name;
        data.name = name;
        data.type = RESOURCE;
        data.resourceFactory = &Resource::create<List<T>>;
        data.getType = List<T>::getTypeStatic;
        data.enumContext = &enumContext;
        data.data = &this->m_data;

        parent->registerExport(data);
    }
}


