#include "utils.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "arrray.hpp"

using Utils::Gadget;

void foo(int);

TEST_CASE("init variables")
{
    int x;
    const int y = 10;
    int z(10);
    int w{10};
    char c{static_cast<char>(w)};
    char c_evil{(char)(w)}; // deprecated - use static_cast instead
}

void print(const Array& arr)
{
    std::cout << "{ ";
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        std::cout << *it << " ";
    }

    std::cout << "}\n";
}

TEST_CASE("dynamic arrays")
{
    SECTION("C")
    {
        void* raw_mem = malloc(10 * sizeof(int));
        int* arr = (int*)(raw_mem);
        arr[0] = 20;
        CHECK(arr[0] == 20);
        free(raw_mem);
    }

    SECTION("C++")
    {
        SECTION("new - unit alloc")
        {
            int* ptr = new int(13); // allocation
            *ptr = 42;
            CHECK(*ptr == 42);
            delete ptr; // de-allocation
        }

        SECTION("new - table alloc")
        {
            int* arr = new int[100];
            arr[0] = 42;
            delete arr;
        }
    }
}

TEST_CASE("containers & iterators")
{
    SECTION("native arrays & pointers")
    {
        const int size = 8;
        int native_arr[size] = {};

        int* start = native_arr;
        int* post_end = native_arr + size;

        for (auto it = start; it != post_end; ++it)
        {
            *it = 42;
        }
    }

    SECTION("container")
    {
        std::list<int> vec = {1, 2, 3, 4, 5};

        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            *it = 42;
        }
    }

    SECTION("range-based for")
    {
        SECTION("native array")
        {
            const int size = 8;
            int native_arr[size] = {};

            for (auto& item : native_arr)
            {
                item = 42;
            }
        }

        SECTION("container")
        {
            std::list<int> vec = {1, 2, 3, 4, 5};

            for (const auto& item : vec)
            {
                std::cout << item << " ";
            }
            std::cout << "\n";

            SECTION("is interpreted as")
            {
                auto start = vec.begin();
                auto post_end = vec.end();

                for (auto it = start; it != post_end; ++it)
                {
                    const auto& item = *it;
                    std::cout << item << " ";
                }
            }
        }

        SECTION("Array")
        {
            Array arr = {1, 2, 3, 4, 5, 6};

            for (auto& item : arr)
            {
                item = 2 * item;
            }

            for (const auto& item : arr)
            {
                std::cout << item << " ";
            }
            std::cout << "\n";
        }
    }
}

TEST_CASE("Array - basic interface")
{
    Array data1{10};

    int value = data1[0];

    data1[0] = 42;
    data1.at(1) = 665;
    CHECK_THROWS_AS(data1.at(100), std::out_of_range);

    print(data1);

    CHECK(data1.size() == 10);

    for (int i = 0; i < data1.size(); ++i)
    {
        data1[i] = i * i;
    }

    CHECK(data1[3] == 4);

    std::fill_n(data1.begin(), 4, 1024);
}

struct Data
{
    int id;
    std::string name;
};

TEST_CASE("copy")
{
    SECTION("Data")
    {
        Data d1{42, "data"};
        Data d2 = d1; // copy
        CHECK(&d1 != &d2);
        CHECK(d1.id == d2.id);
        CHECK(d1.name == d2.name);

        Data d3{d1}; // copy
    }

    SECTION("Array")
    {
        Array data1 = {1, 2, 3, 4, 5, 6, 7, 8};
        CHECK(data1.size() == 8);

        Array data2 = data1; // copy constructor
        CHECK(data2.size() == 8);
        CHECK(data2[2] == 3);

        Array data3(data1); // copy constructor
        CHECK(data1 == data3);

        Array data4 = {1, 2, 3};
        CHECK(data1 != data4); // !(data1 == data4)

        data4 = data1; // copy assignment
        CHECK(data1 == data4);

        data1 = data1;

        Array data5 = {665, 667};

        SECTION("swap")
        {
            data1.swap(data5);
            CHECK(data1 == Array{665, 667});
            CHECK(data5 == Array{1, 2, 3, 4, 5, 6, 7, 8});
        }
    }
}

Array create_squares(size_t size, int start = 1)
{
    Array squares(size);

    auto it = squares.begin();
    for (int i = start; it != squares.end(); ++it, ++i)
    {
        *it = i * i;
    }

    return squares;
}

TEST_CASE("return by value")
{
    SECTION("return")
    {
        Array squares = create_squares(5);
        CHECK(squares == Array{1, 4, 9, 16, 25});

        Array backup = squares;
    }

    SECTION("push_backs")
    {
        Array squares = create_squares(5);

        std::vector<Array> data;
        //data.reserve(100);
        data.push_back(squares);
        data.push_back(Array{665, 667});
        data.push_back(Array{1115, 5667});
        data.push_back(Array{666, 999});

        Array row = {1, 2, 3, 4, 5, 6};
        data.push_back(std::move(row));

        row = Array{0, 0, 0};
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Rule of zero

struct Person
{
    int id;
    std::string name;
    Array data;

    Person(int id, std::string name, Array data)
        : id{id}, name{std::move(name)}, data{std::move(data)}
    {}

    void print() const
    {
        std::cout << "Person: " << id << "; " << name << "; size: " << data.size() << "\n"; 
    }
};

TEST_CASE("rule of zero")
{
    SECTION("copy")
    {
        Person p1{42, "Jan", Array{1, 2, 3}};
        Person p2{66, "Adam", Array{53, 52, 51}};
        
        Person p3 = p1; // cc        
        p1 = p2;        // cc=
    }

    SECTION("move")
    {
        Person p1{42, "Jan", Array{1, 2, 3}};
        Person p2{66, "Adam", Array{53, 52, 51}};
        
        Person p3 = std::move(p1);  // mv
        p1 = Person(665, "LessEvil", Array{665, 667}); // mv=
        p2 = std::move(p3); // mv        
    }
}

struct Human
{
    int id = 0;
    std::string name;

    Human() = default;

    Human(int id, std::string name) : id{id}, name{std::move(name)}
    {}

    Human(const Human&) = default;
    Human& operator=(const Human&) = default;

    Human(Human&&) = default;
    Human& operator=(Human&&) = default;

    ~Human() { std::cout << "Log: ~Human(" << id  << ")\n"; }

    void print() const
    {
        std::cout << "Person(" << id << ", '" << name << "')\n";
    }
};

TEST_CASE("default special functions")
{
    Human p_def{42, "Jan"};
    p_def.print();

    Human p_twin = p_def;
    p_twin.print();

    Human p_target = std::move(p_twin); // static_cast<Human&&>(p_twin)
    p_target.print();

    CHECK(p_twin.name == "");
}

TEST_CASE("templated Array")
{
    Templates::Array<int> data_ints = {1, 2, 3, 4, 5};
    CHECK(data_ints[2] == 3);

    Templates::Array<std::string> data_strings = { "one", "two", "three" };
    CHECK(data_strings[1] == "two");
}