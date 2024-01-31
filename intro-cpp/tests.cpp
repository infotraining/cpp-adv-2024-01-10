#include "utils.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

using Utils::Gadget;

class Array
{
public:
    using iterator = int*;             // typedef int* iterator;
    using const_iterator = const int*; // typedef const Pint* iterator;

    Array(size_t size)
        : size_{size}
        , items_{new int[size]}
    {
        // for(size_t i = 0; i < size_; ++i)
        //     items_[i] = 0; // *(items_ + i) = 0

        std::fill_n(items_, size_, 0);
    }

    Array(std::initializer_list<int> il)
        : size_{il.size()}
        , items_{new int[size_]}
    {
        std::copy(il.begin(), il.end(), items_);
    }

    // copy constructor
    Array(const Array& source)
        : size_{source.size()}
        , items_{new int[source.size()]}
    {
        std::copy(source.begin(), source.end(), items_);
    }

    // copy assignment operator
    Array& operator=(const Array& source)
    {
        // if (this != &source)
        // {
        //     size_ = source.size_;
        //     delete[] items_;
        //     items_ = new int[size_];

        //     std::copy(source.begin(), source.end(), items_);
        // }

        Array temp(source);
        swap(temp);

        return *this;
    }

    void swap(Array& other)
    {
        std::swap(size_, other.size_);
        std::swap(items_, other.items_);
    }

    ~Array()
    {
        delete[] items_;
    }

    size_t size() const
    {
        return size_;
    }

    iterator begin()
    {
        return items_;
    }

    const_iterator begin() const
    {
        return items_;
    }

    iterator end()
    {
        return items_ + size_;
    }

    const_iterator end() const
    {
        return items_ + size_;
    }

    const int& operator[](size_t index) const // read-only
    {
        return items_[index];
    }

    int& operator[](size_t index) // read-write
    {
        return items_[index];
    }

    const int& at(size_t index) const // read-only
    {
        if (index >= size_)
            throw std::out_of_range("Index out of bounds");

        return items_[index];
    }

    int& at(size_t index) // read-write
    {
        if (index >= size_)
            throw std::out_of_range("Index out of bounds");

        return items_[index];
    }

    bool operator==(const Array& rhs) const
    {
        return size_ == rhs.size_ && std::equal(begin(), end(), rhs.begin());
    }

    bool operator!=(const Array& rhs) const
    {
        return !(*this == rhs);
    }

private:
    size_t size_;
    int* items_;
};

void foo(int);

TEST_CASE("init variables")
{
    int x;
    const int y = 10;
    int z(10);
    int w{10};
    char c{static_cast<char>(w)};
    char c_evil{(char)(w)}; // deprecated
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
    for(int i = start; it != squares.end(); ++it, ++i)
    {
        *it = i * i;
    }

    return squares;
}

TEST_CASE("return by value")
{
    Array squares = create_squares(5);
    CHECK(squares == Array{1, 4, 9, 16, 25});
}