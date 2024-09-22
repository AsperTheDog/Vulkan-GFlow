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

        void setValues(const T& first, const U& second)
        {
            *this->first = first;
            *this->second = second;
        }

        [[nodiscard]] T getFirst() const { return *first; }
        [[nodiscard]] U getSecond() const { return *second; }

        bool operator==(const Pair& other) const { return *other.first == *first && *other.second == *second; }

        template <typename V>
        friend class List;
    };

    template <typename T, typename U>
    class ResPair final : public Resource
    {
        EXPORT_RESOURCE(T, first);
        EXPORT_RESOURCE(U, second);

    public:
        DECLARE_RESOURCE(ResPair)

        T* getFirst() { return *first; }
        U* getSecond() { return *second; }

        template <typename V>
        friend class List;
    };

    typedef Pair<int, int> IntPair;
    typedef Pair<size_t, size_t> BigIntPair;
}


