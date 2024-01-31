#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace std::literals;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        std::string& lref_name = name;
        const std::string& clref_full_name = full_name(name, "Kowalski");
        //clref_full_name[0] = 'J';
    }

    SECTION("C++11")
    {
        std::string&& rref_full_name = full_name(name, "Kowalski");
        rref_full_name[0] = 'J';

        CHECK(rref_full_name == "Jan Kowalski");

        //std::string&& rref_name = name; // ERROR
    }
}