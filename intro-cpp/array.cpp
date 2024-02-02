#include "arrray.hpp"

Array::Array(size_t size)
    : size_{size}
    , items_{new int[size]}
{
    // for(size_t i = 0; i < size_; ++i)
    //     items_[i] = 0; // *(items_ + i) = 0

    std::fill_n(items_, size_, 0);
}

Array::Array(std::initializer_list<int> il)
    : size_{il.size()}
    , items_{new int[size_]}
{
    std::copy(il.begin(), il.end(), items_);

    print();
}