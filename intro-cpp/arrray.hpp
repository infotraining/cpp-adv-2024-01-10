#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <iostream>

class Array
{
public:
    using iterator = int*;             // typedef int* iterator;
    using const_iterator = const int*; // typedef const Pint* iterator;

    explicit Array(size_t size);
    Array(std::initializer_list<int> il);        

    // copy constructor
    Array(const Array& source)
        : size_{source.size()}
        , items_{new int[source.size()]}
    {
        std::copy(source.begin(), source.end(), items_);

        std::cout << "CC: ";
        print();
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

        std::cout << "CC op=: ";
        print();

        return *this;
    }

    // move constructor
    Array(Array&& source) noexcept
        : size_{source.size_}
        , items_{source.items_}
    {
        source.size_ = 0; // extra safety
        source.items_ = nullptr;

        try
        {
            std::cout << "MV: ";
            print();
        }
        catch (...)
        { }
    }

    // move assignment
    Array& operator=(Array&& source)
    {
        // if (this != &source)
        // {
        //     delete[] items_;

        //     size_ = source.size_;
        //     items_ = source.items_;

        //     source.size_ = 0; // extra safety
        //     source.items_ = nullptr;
        // }

        Array temp = std::move(source);
        swap(temp);

        std::cout << "MV op=: ";
        print();

        return *this;
    }

    void swap(Array& other)
    {
        std::swap(size_, other.size_);
        std::swap(items_, other.items_);
    }

    ~Array() noexcept
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

    void print() const
    {
        std::cout << "Array{ ";
        for (const auto& item : *this)
        {
            std::cout << item << " ";
        }
        std::cout << "}\n";
    }
};

namespace Templates
{
    template <typename T>
    class Array
    {
    public:
        using iterator = T*;             // typedef int* iterator;
        using const_iterator = const T*; // typedef const Pint* iterator;

        explicit Array(size_t size)
            : size_{size}
            , items_{new T[size]}
        {
            std::fill_n(items_, size_, T{});
        }

        Array(std::initializer_list<T> il)
            : size_{il.size()}
            , items_{new T[size_]}
        {
            std::copy(il.begin(), il.end(), items_);

            print();
        }

        // copy constructor
        template <typename U>
        Array(const Array<U>& source)
            : size_{source.size()}
            , items_{new T[source.size()]}
        {
            std::copy(source.begin(), source.end(), items_);

            std::cout << "CC: ";
            print();
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

            std::cout << "CC op=: ";
            print();

            return *this;
        }

        // move constructor
        Array(Array&& source) noexcept
            : size_{source.size_}
            , items_{source.items_}
        {
            source.size_ = 0; // extra safety
            source.items_ = nullptr;

            try
            {
                std::cout << "MV: ";
                print();
            }
            catch (...)
            { }
        }

        // move assignment
        Array& operator=(Array&& source)
        {
            Array temp = std::move(source);
            swap(temp);

            std::cout << "MV op=: ";
            print();

            return *this;
        }

        void swap(Array& other)
        {
            std::swap(size_, other.size_);
            std::swap(items_, other.items_);
        }

        ~Array() noexcept
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

        const T& operator[](size_t index) const // read-only
        {
            return items_[index];
        }

        T& operator[](size_t index) // read-write
        {
            return items_[index];
        }

        const T& at(size_t index) const // read-only
        {
            if (index >= size_)
                throw std::out_of_range("Index out of bounds");

            return items_[index];
        }

        T& at(size_t index) // read-write
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
        T* items_;

        void print() const;
    };

    template <typename T>
    void Array<T>::print() const
    {
        std::cout << "Array{ ";
        for (const auto& item : *this)
        {
            std::cout << item << " ";
        }
        std::cout << "}\n";
    }

} // namespace Templates

#endif