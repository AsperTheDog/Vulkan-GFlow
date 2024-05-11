#pragma once
#include "resource.hpp"

namespace gflow::parser
{
    class Pipeline final : public Resource
    {
    public:
        Pipeline(std::string_view path, Project* parent);

        [[nodiscard]] std::string getType() const override;

    private:
        EXPORT(std::string, name);
        EXPORT_GROUP(tg1, "Test Group 1");
        EXPORT(int, testInt);
        EXPORT(bool, testBoot);
        EXPORT(float, testFloat);
        EXPORT(Ref, testRef);
        EXPORT_GROUP(tg2, "Test Group 2");
        EXPORT(std::string, testString);
        
        static Resource* create(const std::string_view path, Project* parent) { return new Pipeline(path, parent); }

        friend class Project;
    };
}


