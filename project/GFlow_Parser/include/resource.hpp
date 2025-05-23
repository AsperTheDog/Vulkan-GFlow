#pragma once
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "enum_contexts.hpp"
#include "string_helper.hpp"
#include "utils/logger.hpp"

#define EXPORT(type, name) gflow::parser::Export<type, false, false> ##name{#name, this}
#define EXPORT_GROUP(name, title) gflow::parser::Export<bool, false, false> _##name{title, this, true}
#define EXPORT_ENUM(name, context) gflow::parser::Export<gflow::parser::EnumExport, false, false> ##name{#name, this, context}
#define EXPORT_BITMASK(name, context) gflow::parser::Export<gflow::parser::EnumBitmask, false, false> ##name{#name, this, context}
#define EXPORT_RESOURCE(type, name, createOnInit, IsRef) gflow::parser::Export<type*, createOnInit, IsRef> ##name{#name, this}

#define DECLARE_RESOURCE_ANCESTOR_NO_CONST(type, parent)                                     \
        [[nodiscard]] static std::string getTypeStatic() { return #type; }                   \
        [[nodiscard]] std::string getType() const override { return #type; }                 \
        static Resource* create(const std::string& path, Resource::ExportData* metadata)     \
        {                                                                                    \
            ##type* newRes = new type(path);                                                 \
            newRes->initContext(metadata);                                                   \
            return newRes;                                                                   \
        }                                                                                    \
        friend class ResourceManager;                                                        \
        template <typename U, bool C, bool R> friend class Export;                           \
        friend class Resource;

#define DECLARE_RESOURCE_ANCESTOR(type, parent)                                              \
        explicit type(const std::string& path) : parent(path) {}                             \
        DECLARE_RESOURCE_ANCESTOR_NO_CONST(type, parent)                                           

#define DECLARE_RESOURCE(type) DECLARE_RESOURCE_ANCESTOR(type, Resource)
#define DECLARE_RESOURCE_NO_CONST(type) DECLARE_RESOURCE_ANCESTOR_NO_CONST(type, Resource)

namespace gflow::parser
{
    class Project;
    class Resource;

    enum DataType : uint8_t
    {
        NONE,
        STRING,
        INT,
        BIGINT,
        FLOAT,
        BOOL,
        VEC2,
        VEC3,
        VEC4,
        MAT3,
        MAT4,
        COLOR,
        UCOLOR,
        FILE,
        ENUM,
        ENUM_BITMASK,
        RESOURCE
    };

    enum DataUsage : uint8_t
    {
        USED,
        READ_ONLY,
        NOT_USED
    };

    struct EnumExport { uint32_t id; };
    struct EnumBitmask { uint32_t mask; };
    struct FilePath { std::string path; };

    struct Vec2
    {
        float x, y;
        bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }

        [[nodiscard]] std::string toString() const { return std::to_string(x) + ", " + std::to_string(y); }
    };
    struct Vec3
    {
        float x, y, z;
        bool operator==(const Vec3& other) const { return x == other.x && y == other.y && z == other.z; }

        [[nodiscard]] std::string toString() const { return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z); }
    };
    struct Vec4
    {
        float x, y, z, w;
        bool operator==(const Vec4& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }

        [[nodiscard]] std::string toString() const { return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w); }
    };
    struct Color
    {
        float r, g, b, a;
        bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
        [[nodiscard]] std::string toString() const { return std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a); }
    };
    struct UColor
    {
        uint8_t r, g, b, a;
        bool operator==(const UColor& other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
        [[nodiscard]] std::string toString() const { return std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", " + std::to_string(a); }
        [[nodiscard]] gflow::parser::Color getfColor() const { return { r / 255.f, g / 255.f, b / 255.f, a / 255.f }; }
        void fromfColor(const Color& ftmp)
        {
            r = static_cast<uint8_t>(ftmp.r * 255);
            g = static_cast<uint8_t>(ftmp.g * 255);
            b = static_cast<uint8_t>(ftmp.b * 255);
            a = static_cast<uint8_t>(ftmp.a * 255);
        }
    };
    struct Mat3
    {
        float data[9];
        bool operator==(const Mat3& other) const
        {
            for (int i = 0; i < 9; i++)
                if (data[i] != other.data[i]) return false;
            return true;
        }

        [[nodiscard]] std::string toString() const
        {
            std::string str;
            for (int i = 0; i < 9; i++)
                str += std::to_string(data[i]) + ", ";
            return str;
        }
    };
    struct Mat4
    {
        float data[16];
        bool operator==(const Mat4& other) const
        {
            for (int i = 0; i < 16; i++)
                if (data[i] != other.data[i]) return false;
            return true;
        }

        [[nodiscard]] std::string toString() const
        {
            std::string str;
            for (int i = 0; i < 16; i++)
                str += std::to_string(data[i]) + ", ";
            return str;
        }
    };

    // Utility class with full context of a Resource, used by GFlow when reacting to specific events
    struct ResourceElemPath
    {
        std::string path;
        gflow::parser::Resource* parentResource;
        std::string element;
        std::string stackedPath;

        bool pointsToResource(const gflow::parser::Resource* parent, const std::string& elem) const
        {
            return this->parentResource == parent && this->element == elem;
        }

        bool operator==(const ResourceElemPath& other) const
        {
            return path == other.path && parentResource == other.parentResource && element == other.element && stackedPath == other.stackedPath;
        }
    };

    // Main class that represents a variable that can be serialized. Must be used to hold data inside a Resource. 
    // It is recommended to use the utility EXPORT macros defined at the top of this file
    template <typename T, bool CreateOnInit, bool IsRef>
    class Export
    {
    public:
        Export(const std::string& name, Resource* parent, bool group = false);
        Export(const std::string& name, Resource* parent, EnumContext& enumContext);

        const T& operator*() const { return m_data; }
        T& operator*() { return m_data; }

        [[nodiscard]] Resource* getParent() const { return m_parent; }
        void setData(T value);

    private:
        bool m_isGroup = false;
        std::string m_name;
        T m_data;

        Resource* m_parent;
    };

    // Main class of the system. Any created Resource must inherit this class to get serialization and reflection capabilities.
    class Resource
    {
    public:
        struct ExportData;
        using ResourceFactory = std::function<Resource* (const std::string&, Resource::ExportData*)>;

        // Struct containing all the runtime data in an exported variable. A lot of extra data has been added to
        // allow for extra functionality needed by GFlow
        struct ExportData
        {
            DataType type = NONE;
            std::string name{};
            void* data = nullptr;
            EnumContext* enumContext = nullptr;
            ResourceFactory resourceFactory = nullptr;
            Resource* parent = nullptr;
            std::string (*getType)() = nullptr;
            bool isRef = false;
        };

        // Utility struct used in serialization process
        struct SerializedResourceEntry
        {
            uint32_t key;
            std::string type;
            bool isSubresource;
            std::unordered_map<std::string, std::string> data;
        };

        typedef std::unordered_map<uint32_t, SerializedResourceEntry> SerializedResourceEntries;

    public:
        Resource() { setID(0); }
        virtual ~Resource() = default;

        struct Ref { std::string path; };

        // This function can be overridden to provide custom functionality when loaded
        virtual void initContext(ExportData* metadata) {}
        
        // Serialization functions. While default behavior is normally enough for most Resources, they can be overridden to provide 
        // custom behavior (see GFlow_Parser/include/resource/list.hpp)
        virtual std::string serialize();
        virtual void deserialize(const SerializedResourceEntry& data, const SerializedResourceEntries& dependencies);

        bool deserialize(std::string filename = "");

        // These getters and setters can provide runtime retrieval of exported parameters in a resource dynamically
        virtual std::pair<std::string, std::string> get(const std::string& variable);
        virtual bool set(const std::string& variable, const std::string& value, const SerializedResourceEntries& dependencies = {});
        virtual DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath = {}) { return USED; }

        template <typename T>
        T getValue(const std::string& variable);

        void initializeExport(const std::string& name);

        // These functions can be overridden to provide custom behavior when these events happen
        virtual void exportsChanged() {}
        virtual void exportChanged(const std::string& variable) { exportsChanged(); }

        // This function can be overridden to support dynamic export parameters. This is used for example by the list resource to dynamically 
        // serialize all elements in its internal array (see GFlow_Parser/include/resource/list.hpp)
        [[nodiscard]] virtual std::vector<ExportData> getCustomExports() { return {}; }

        [[nodiscard]] std::vector<ExportData> getExports();

        // Only function required to be overridden, Needed for the serialization system to properly identify subresource types. 
        // Implemented automatically by the DECLARE macros
        [[nodiscard]] virtual std::string getType() const = 0;

        [[nodiscard]] std::string getPath() const { return m_path; }
        [[nodiscard]] std::string getMetaPath() const;
        [[nodiscard]] uint32_t getID() const { return m_id; }
        [[nodiscard]] bool isSubresource() const { return m_path.empty(); }

        [[nodiscard]] bool isNull(const std::string& variable);

        template <typename T>
        static Resource* create(const std::string& path, ExportData* metadata);

    protected:
        explicit Resource(std::string path) : m_path(std::move(path)) { setID(0); }

        std::string m_path;
        uint32_t m_id;

        std::vector<ExportData> m_exports;

        void registerExport(const ExportData& data) { m_exports.push_back(data); }

    private:
        void setID(uint32_t id = 0);

        inline static std::unordered_set<uint32_t> s_ids;

        template <typename T, bool C, bool R>
        friend class Export;

        friend class ResourceManager;
        friend class Project;
    };

    // In GFlow, Resources are managed by a manager class. Implementing this function here avoids some annoying circular dependencies
    Resource* createResourceInManager(const Resource::ResourceFactory& factory, Resource::ExportData* data = nullptr);

    //***************************************************************
    //*********************** Implementations ***********************
    //***************************************************************

    template <typename T, bool C, bool R>
    Export<T, C, R>::Export(const std::string& name, Resource* parent, const bool group) : m_data{}, m_parent(parent)
    {
        m_isGroup = group;

        Resource::ExportData exportData;
        exportData.name = name;

        if (m_isGroup)
        {
            exportData.type = BOOL;
            exportData.data = nullptr;
            parent->registerExport(exportData);
            return;
        }

        exportData.data = &m_data;
        exportData.isRef = R;

        if constexpr (std::is_same_v<T, std::string>) exportData.type = STRING;
        else if constexpr (std::is_same_v<T, FilePath>) exportData.type = FILE;
        else if constexpr (std::is_same_v<T, int>) exportData.type = INT;
        else if constexpr (std::is_same_v<T, size_t>) exportData.type = BIGINT;
        else if constexpr (std::is_same_v<T, float>) exportData.type = FLOAT;
        else if constexpr (std::is_same_v<T, bool>) exportData.type = BOOL;
        else if constexpr (std::is_same_v<T, Vec2>) exportData.type = VEC2;
        else if constexpr (std::is_same_v<T, Vec3>) exportData.type = VEC3;
        else if constexpr (std::is_same_v<T, Vec4>) exportData.type = VEC4;
        else if constexpr (std::is_same_v<T, Mat3>) exportData.type = MAT3;
        else if constexpr (std::is_same_v<T, Mat4>) exportData.type = MAT4;
        else if constexpr (std::is_same_v<T, Color>) exportData.type = COLOR;
        else if constexpr (std::is_same_v<T, UColor>) exportData.type = UCOLOR;
        else if constexpr (std::is_pointer_v<T> && std::is_base_of_v<Resource, std::remove_pointer_t<T>>)
        {
            exportData.type = RESOURCE;
            exportData.resourceFactory = &Resource::create<std::remove_pointer_t<T>>;
            exportData.getType = std::remove_pointer_t<T>::getTypeStatic;
            if constexpr (C)
                *static_cast<Resource**>(exportData.data) = createResourceInManager(exportData.resourceFactory);
        }
        else
        {
            exportData.type = NONE;
            LOG_ERR("Export type not supported");
            return;
        }
        parent->registerExport(exportData);
    }

    template <typename T, bool C, bool R>
    Export<T, C, R>::Export(const std::string& name, Resource* parent, EnumContext& enumContext) : m_parent(parent)
    {
        Resource::ExportData data;
        this->m_name = name;
        data.name = name;
        data.enumContext = &enumContext;
        data.data = &this->m_data;
        data.isRef = R;
        if constexpr (std::is_same_v<T, EnumExport>) data.type = ENUM;
        else if constexpr (std::is_same_v<T, EnumBitmask>) data.type = ENUM_BITMASK;
        else
        {
            data.type = NONE;
            LOG_ERR("Export type not supported for enum export");
            return;
        }
        parent->registerExport(data);
    }

    template <typename T, bool C, bool R>
    void Export<T, C, R>::setData(T value)
    {
         m_data = value;
         m_parent->exportChanged(m_name);
    }

    template <typename T>
    T Resource::getValue(const std::string& variable)
    {
        for (const ExportData& exportData : m_exports)
        {
            if (exportData.name == variable)
            {
                return *static_cast<T*>(exportData.data);
            }
        }
        for (const ExportData& exportData : getCustomExports())
        {
            if (exportData.name == variable)
            {
                return *static_cast<T*>(exportData.data);
            }
        }
        throw std::runtime_error("Variable not found");
    }

    template <typename T>
    Resource* Resource::create(const std::string& path, ExportData* metadata)
    {
        T* newRes = new T(path);
        newRes->initContext(metadata);
        return newRes;
    }
}

