#pragma once
#include "../resource_manager.hpp"

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

    template <typename T, typename U, typename V>
    class Triple final : public Resource
    {
        EXPORT(T, first);
        EXPORT(U, second);
        EXPORT(V, third);

    public:
        DECLARE_RESOURCE(Triple)

        void setValues(const T& first, const U& second, const V& third)
        {
            *this->first = first;
            *this->second = second;
            *this->third = third;
        }
        [[nodiscard]] T getFirst() const { return *first; }
        [[nodiscard]] U getSecond() const { return *second; }
        [[nodiscard]] V getThird() const { return *third; }

        bool operator==(const Triple& other) const { return *other.first == *first && *other.second == *second && *other.third == *third; }

        template <typename W>
        friend class List;
    };

    template <typename T, typename U, typename V>
    class ResTriple final : public Resource
    {
        EXPORT_RESOURCE(T, first);
        EXPORT_RESOURCE(U, second);
        EXPORT_RESOURCE(V, third);

    public:
        DECLARE_RESOURCE(ResTriple)

        T* getFirst() { return *first; }
        U* getSecond() { return *second; }
        V* getThird() { return *third; }

        template <typename W>
        friend class List;
    };
}


