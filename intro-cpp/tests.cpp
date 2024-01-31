#include "utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

using Utils::Gadget;

class Array
{
public:
    Array(size_t size)
        : size_{size}, items_{(int*)malloc(size_ * sizeof(int))}
    {
        // for(size_t i = 0; i < size_; ++i)
        //     items_[i] = 0; // *(items_ + i) = 0
        
        std::fill_n(items_, size_, 0);
    }

    ~Array()
    {
        free(items_);
    }

    size_t size() const
    {
        return size_;
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

private:
    const size_t size_;
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
    for (int i = 0; i < arr.size(); ++i)
    {
        std::cout << arr[i] << " ";
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

    // for(int i = 0; i < data1.size(); ++i)
    // {
    //     data1[i] = i * i;
    // }

    // CHECK(data1[3] == 4);
}