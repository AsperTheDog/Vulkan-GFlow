#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace gflow::parser
{
    struct EnumContext
    {
        std::vector<const char*> names;
        std::vector<uint32_t> values;

        uint32_t operator[](const std::string& name) const
        {
            for (uint32_t i = 0; i < names.size(); i++)
            {
                if (names[i] == name)
                    return values[i];
            }
            return 0;
        }
    };

    struct EnumContexts
    {
        static EnumContext format;
        static EnumContext primitiveTopology;
        static EnumContext polygonMode;
        static EnumContext cullMode;
        static EnumContext frontFace;
        static EnumContext compareOp;
        static EnumContext blendFactor;
        static EnumContext blendOp;
        static EnumContext colorWriteMaskBits;
        static EnumContext RenderpassNodeType;
        static EnumContext ImageUsageContext;
        static EnumContext attachmentType;
        static EnumContext PushConstantElement;
        static EnumContext ImageSource;
        static EnumContext ModelFields;
        static EnumContext ExecutionImageType;
    };
}
