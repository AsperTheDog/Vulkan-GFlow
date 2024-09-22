#pragma once
#include "../resource.hpp"

namespace gflow::parser
{
    template <typename T, typename U>
    class Pair final : public Resource
    {
        EXPORT(T, first);
        EXPORT(U, second);

    public:
        DECLARE_RESOURCE(Pair)

        template <typename V>
        friend class List;
    };

    typedef Pair<int, int> IntPair;
}


