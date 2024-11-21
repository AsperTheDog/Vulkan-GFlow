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

        void setValues(const T& firstVal, const U& secondVal)
        {
            *this->first = firstVal;
            *this->second = secondVal;
        }

        [[nodiscard]] T getFirst() const { return *first; }
        [[nodiscard]] U getSecond() const { return *second; }

        bool operator==(const Pair& other) const { return *other.first == *first && *other.second == *second; }

        template <typename V>
        friend class List;
    };

    template <typename T, typename U, bool C = true>
    class ResPair final : public Resource
    {
        EXPORT_RESOURCE(T, first, C);
        EXPORT_RESOURCE(U, second, C);

    public:
        DECLARE_RESOURCE(ResPair)

        T* getFirst() { return *first; }
        U* getSecond() { return *second; }

        template <typename V>
        friend class List;
    };

    typedef Pair<int, int> IntPair;
    typedef Pair<size_t, size_t> BigIntPair;
    typedef Pair<float, float> FloatPair;
    typedef Pair<std::string, std::string> StringPair;

    template <typename T, typename U, typename V>
    class Triple final : public Resource
    {
        EXPORT(T, first);
        EXPORT(U, second);
        EXPORT(V, third);

    public:
        DECLARE_RESOURCE(Triple)

        void setValues(const T& firstVal, const U& secondVal, const V& thirdVal)
        {
            *this->first = firstVal;
            *this->second = secondVal;
            *this->third = thirdVal;
        }
        [[nodiscard]] T getFirst() const { return *first; }
        [[nodiscard]] U getSecond() const { return *second; }
        [[nodiscard]] V getThird() const { return *third; }

        bool operator==(const Triple& other) const { return *other.first == *first && *other.second == *second && *other.third == *third; }

        template <typename W>
        friend class List;
    };

    template <typename T, typename U, typename V, bool C = true>
    class ResTriple final : public Resource
    {
        EXPORT_RESOURCE(T, first, C);
        EXPORT_RESOURCE(U, second, C);
        EXPORT_RESOURCE(V, third, C);

    public:
        DECLARE_RESOURCE(ResTriple)

        T* getFirst() { return *first; }
        U* getSecond() { return *second; }
        V* getThird() { return *third; }

        template <typename W>
        friend class List;
    };
}


