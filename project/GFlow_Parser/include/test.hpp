#pragma once
#include "resource.hpp"

namespace gflow::parser
{
    class Test final : public Resource
    {
    public:
        [[nodiscard]] std::string getType() const override { return "Test"; }

    private:
        EXPORT(std::string, name);
        EXPORT_GROUP(gp1, "Group 1");
        EXPORT(int, random1);
        EXPORT(int, random2);
        EXPORT(int, random3);
        EXPORT(int, random4);
        EXPORT_GROUP(gp2, "Group 2");
        EXPORT(int, random5);
        EXPORT(int, random6);
        EXPORT(int, random7);

        Test(const std::string& path, Project* parent) : Resource(path, parent) {}
        static Resource* create(const std::string& path, Project* parent) { return new Test(path, parent); }

        template <typename T>
        friend class Export;
    };
}


