#pragma once
#include <cstdint>
#include <vector>

namespace gflow::parser
{
    struct EnumContext
    {
        std::vector<const char*> names;
        std::vector<uint32_t> values;
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
    };
}
