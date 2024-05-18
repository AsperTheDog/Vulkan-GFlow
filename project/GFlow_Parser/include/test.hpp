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
        EXPORT(Test*, test);
        EXPORT_GROUP(gp1, "Group 1");
        EXPORT(int, random1);
        EXPORT(bool, random2);
        EXPORT(int, random3);
        EXPORT(bool, random4);
        EXPORT_GROUP(gp2, "Group 2");
        EXPORT(int, random5);
        EXPORT(bool, random6);
        EXPORT(int, random7);

        explicit Test(const std::string& path) : Resource(path) {}
        static Resource* create(const std::string& path) { return new Test(path); }

        template <typename T>
        friend class Export;
    };
}


