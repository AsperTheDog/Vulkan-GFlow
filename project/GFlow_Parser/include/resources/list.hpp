#pragma once
#include "../resource_manager.hpp"

#define EXPORT_LIST(type, name) gflow::parser::Export<gflow::parser::List<type>*> ##name{#name, this}
#define EXPORT_ENUM_LIST(name, context) gflow::parser::Export<gflow::parser::List<gflow::parser::EnumExport>*> ##name{#name, this, context}
#define EXPORT_RESOURCE_LIST(type, name) gflow::parser::Export<gflow::parser::List<type*>*> ##name{#name, this}

namespace gflow::parser
{
    // template cannot be bool
    template <typename T>
    class List : public Resource
    {
    public:
        [[nodiscard]] EnumContext* getEnumContext() const { return m_enumContext; }

        [[nodiscard]] int size() const { return m_size; }
        [[nodiscard]] const T& operator[](int index) const { return m_data[index]; }
        [[nodiscard]] T& operator[](int index) { return m_data[index]; }

        void initContext(ExportData* metadata) override;

        void setEnumContext(EnumContext* enumContext) { m_enumContext = enumContext; }
        void setReadonly(const bool readonly) { m_readonlySize = readonly; }

        bool set(const std::string& variable, const std::string& value, const ResourceEntries& dependencies) override;
        [[nodiscard]] std::vector<ExportData> getCustomExports() override;

        void exportsChanged() override;
        void exportChanged(const std::string& variable) override;

        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) override;

        [[nodiscard]] const std::vector<T>& data() const { return m_data; }

        void remove(int index) { m_data.erase(m_data.begin() + index); m_size--; }
        void erase(T value) { m_data.erase(std::remove(m_data.begin(), m_data.end(), value), m_data.end()); m_size--; }
        void push_back(T value) { m_data.push_back(value); m_size++; }

        T* emplace_back(bool createRecursive = true);

        template<typename U>
        U* emplace_subclass_back(bool createRecursive = true);

    private:
        int m_size = 0;
        std::vector<T> m_data;
        EnumContext* m_enumContext = nullptr;
        bool m_readonlySize = false;

        Resource* m_parent = nullptr;

        ExportData createElemExportData();
        
    public:
        DECLARE_RESOURCE(List)

    private:
        static_assert(!std::is_same_v<T, bool>, "bool is not allowed as the template parameter for MyClass");
    };

    template <typename T>
    void List<T>::initContext(ExportData* metadata)
    {
        if (!metadata) return;
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
            ExportData data = createElemExportData();
            data.data = &m_data[i];
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
    DataUsage List<T>::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (m_readonlySize && variable == "size")
            return READ_ONLY;
        return USED;
    }

    template <typename T>
    T* List<T>::emplace_back(const bool createRecursive)
    {
        if constexpr (std::is_pointer_v<T> && std::is_base_of_v<Resource, std::remove_pointer_t<T>>)
        {
            ExportData elem = createElemExportData();
            elem.name = std::to_string(m_size);

            T data = ResourceManager::createResource<std::remove_pointer_t<T>>("", &elem, createRecursive);
            m_data.push_back(data);
        }
        else
        {
            m_data.push_back(T{});
        }
        m_size++;
        return &m_data.back();
    }

    template <typename T>
    template <typename U>
    U* List<T>::emplace_subclass_back(const bool createRecursive)
    {
        static_assert(std::is_base_of_v<std::remove_pointer_t<T>, U>, "U must be a child class of T");
        static_assert(std::is_base_of_v<Resource, std::remove_pointer_t<T>>, "T must be a Resource");
        
        ExportData elem = createElemExportData();
        elem.name = std::to_string(m_size);

        U* data = ResourceManager::createResource<U>("", &elem, createRecursive);
        m_data.push_back(dynamic_cast<T>(data));
        m_size++;
        return dynamic_cast<U*>(m_data.back());
    }

    template <typename T>
    Resource::ExportData List<T>::createElemExportData()
    {
        ExportData data;
        data.data = nullptr;
        data.name = "";
        data.type = NONE;
        data.enumContext = getEnumContext();
        if constexpr (std::is_same_v<T, std::string>) data.type = STRING;
        else if constexpr (std::is_same_v<T, int>) data.type = INT;
        else if constexpr (std::is_same_v<T, size_t>) data.type = BIGINT;
        else if constexpr (std::is_same_v<T, float>) data.type = FLOAT;
        else if constexpr (std::is_same_v<T, bool>) data.type = BOOL;
        else if constexpr (std::is_same_v<T, EnumExport>)
        {
            data.type = ENUM;
        }
        else if constexpr (std::is_pointer_v<T> && std::is_base_of_v<Resource, std::remove_pointer_t<T>>)
        {
            data.type = RESOURCE;
            data.resourceFactory = &Resource::create<std::remove_pointer_t<T>>;
            data.getType = std::remove_pointer_t<T>::getTypeStatic;
        }
        return data;
    }

    template <typename T>
    class Export<List<T>*>
    {
    public:
        Export(std::string_view name, Resource* parent);
        Export(std::string_view name, Resource* parent, EnumContext& enumContext);

        const List<T>& operator*() const { return *m_data; }
        List<T>& operator*() { return *m_data; }

        [[nodiscard]] Resource* getParent() const { return m_parent; }
        void setData(Resource* value) { m_data = dynamic_cast<List<T>*>(value); }

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


