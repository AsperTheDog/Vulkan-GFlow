#pragma once
#include "../resource_manager.hpp"

namespace gflow::parser
{
    class Image final : public Resource
    {
        EXPORT(std::string, imageID);
        EXPORT(int, referenceCount);
        EXPORT_GROUP(g_properties, "Properties");
        EXPORT(bool, matchWindow);
        EXPORT(Vec2, size);
        EXPORT(bool, clear);
        EXPORT_GROUP(g_data, "Data");
        EXPORT(bool, external);
        EXPORT(FilePath, data);

        void initContext(ExportData* metadata) override {}
        DataUsage isUsed(const std::string& variable, const std::vector<Resource*>& parentPath) override;
    public:
        DECLARE_PUBLIC_RESOURCE(Image)

        template <typename T>
        friend class List;
    };

    inline DataUsage Image::isUsed(const std::string& variable, const std::vector<Resource*>& parentPath)
    {
        if (*matchWindow && variable == "size")
            return NOT_USED;
        if (variable == "referenceCount")
            return READ_ONLY;
        if (variable == "data")
        {
            if (*external)
                return NOT_USED;
            return USED;
        }
        return USED;
    }
}
