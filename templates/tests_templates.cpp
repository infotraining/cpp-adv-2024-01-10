#include "utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

using Utils::Gadget;

using namespace std::literals;

template <typename T>
T max_value(T a, T b)
{
    static_assert(!std::is_pointer_v<T>, "T cannot be a pointer type");
    return a < b ? b : a;
}

const char* max_value(const char* a, const char* b)
{
    return std::strcmp(a, b) < 0 ? b : a;
}

TEST_CASE("function templates")
{
    CHECK(max_value(42, 665) == 665);
    CHECK(max_value(42.1, 665.999) == 665.999);
    CHECK(max_value("text"s, "Text"s) == "text"s);
    CHECK(max_value("text", "Text") == "text"s);
    CHECK(max_value<std::string>("text"s, "Text") == "text"s);
}

template<typename T1 = int, typename T2 = int>
struct ValuePair
{
    T1 first;
    T2 second;

    ValuePair(T1 f = T1{}, T2 s = T2{}) : first{f}, second{s}
    {}    

    void print() const
    {
        std::cout << "ValuePair(" << first << ", " << second << ")\n";
    }
};

TEST_CASE("class templates")
{
    ValuePair<int, int> v1{53, 665};
    CHECK(v1.first == 53);

    ValuePair<int, std::string> v2{67, "sixty-seven"};
    v2.print();

    ValuePair<> v3(42, 556);

    SECTION("since C++17")
    {
        ValuePair vp1{42.1, "text"};
        vp1.print();

        ValuePair vp2;
    }
}

TEST_CASE("template aliases")
{
    // TODO
}

TEST_CASE("template variables")
{
    // TODO
}