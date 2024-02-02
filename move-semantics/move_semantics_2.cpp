#include "gadget.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <memory>
#include <numeric>

////////////////////////////////////////////////
// simplified implementation of unique_ptr - only moveable type

using namespace Helpers;

namespace Explain
{
    template <typename T>
    class UniquePtr
    {
        T* ptr_;

    public:
        UniquePtr()
            : ptr_{nullptr}
        { }

        UniquePtr(nullptr_t)
            : ptr_{nullptr}
        { }

        explicit UniquePtr(T* ptr)
            : ptr_{ptr}
        {
        }

        UniquePtr(const UniquePtr& otherPtr) = delete;
        UniquePtr& operator=(const UniquePtr& otherPtr) = delete;

        UniquePtr(UniquePtr&& otherPtr) noexcept
            : ptr_{otherPtr.ptr_}
        {
            otherPtr.ptr_ = nullptr;
        }

        UniquePtr& operator=(UniquePtr&& otherPtr) noexcept
        {
            if (this != &otherPtr)
            {
                delete ptr_;
                ptr_ = otherPtr.ptr_;
                otherPtr.ptr_ = nullptr;
            }

            return *this;
        }

        ~UniquePtr()
        {
            delete ptr_;
        }

        T* get() const
        {
            return ptr_;
        }

        auto operator*() const -> T&
        {
            return *ptr_;
        }

        T* operator->() const
        {
            return ptr_;
        }

        bool operator==(const UniquePtr& other) const
        {
            return ptr_ == other.ptr_;
        }

        explicit operator bool()
        {
            return ptr_ != nullptr;
        }
    };

    // template <typename T>
    // UniquePtr<T> MakeUnique()
    // {
    //     return UniquePtr<T>(new T());
    // }

    template <typename T, typename... TArg1>
    UniquePtr<T> MakeUnique(TArg1&&... arg1)
    {
        return UniquePtr<T>(new T(std::forward<TArg1>(arg1)...));
    }

    // template <typename T, typename TArg1, typename TArg2>
    // UniquePtr<T> MakeUnique(TArg1&& arg1, TArg2&& arg2)
    // {
    //     return UniquePtr<T>(new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)));
    // }

    // template <typename T, typename TArg1, typename TArg2>
    // UniquePtr<T> MakeUnique(TArg1&& arg1, TArg2&& arg2)
    // {
    //     return UniquePtr<T>(new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2)));
    // }
} // namespace Explain

struct TestDestructor
{
    std::function<void()> callback_;

    TestDestructor(std::function<void()> callback)
        : callback_{callback}
    {
    }

    ~TestDestructor()
    {
        callback_();
    }
};

TEST_CASE("move semantics - unique_ptr")
{
    using namespace Explain;

    SECTION("raw pointer")
    {
        Gadget* ptr_g = new Gadget(42, "ipad");
        ptr_g->use();
    } // memory leak

    SECTION("UniquePtr")
    {
        SECTION("construction/destruction")
        {
            std::unique_ptr<Gadget> ptr_g = std::make_unique<Gadget>(42, "ipad");

            bool was_destructor_called = false;
            {
                std::unique_ptr<TestDestructor> ptr_t(new TestDestructor([&was_destructor_called] { was_destructor_called = true; }));
            }
            CHECK(was_destructor_called == true);
        }

        SECTION("get()")
        {
            UniquePtr<Gadget> ptr_g = MakeUnique<Gadget>(42, "ipad");
            Gadget* raw_ptr_g = ptr_g.get();
            CHECK(raw_ptr_g != nullptr);
        }

        SECTION("* - dereference")
        {
            UniquePtr<Gadget> ptr_g(new Gadget(42, "ipad"));
            Gadget& ref_g = *ptr_g;
            (*ptr_g).use();
        }

        SECTION("-> - access to member")
        {
            UniquePtr<Gadget> ptr_g(new Gadget(42, "ipad"));
            ptr_g->use();
        }

        SECTION("copy is disabled")
        {

            UniquePtr<Gadget> ptr_g(new Gadget(42, "ipad"));

            static_assert(!std::is_copy_constructible_v<UniquePtr<Gadget>>, "UniquePtr<Gadget>> is not copyable");
            // UniquePtr<Gadget> other_ptr = ptr_g; // ERROR
        }

        SECTION("move semantics")
        {
            UniquePtr<Gadget> ptr_g(new Gadget(42, "ipad"));
            UniquePtr<Gadget> other_ptr = std::move(ptr_g);
            other_ptr->use();
            CHECK(ptr_g.get() == nullptr);

            UniquePtr<Gadget> alt_ptr_g(new Gadget(888, "smartwatch"));
            other_ptr = std::move(alt_ptr_g);
            CHECK(other_ptr->id == 888);
        }

        SECTION("nullptr")
        {
            UniquePtr<int> ptr1 = nullptr;
            UniquePtr<int> ptr2{};
            UniquePtr<int> ptr3 = 0;

            CHECK(ptr1.get() == ptr2.get());
            CHECK(ptr1 == ptr2);

            if (ptr1)
                std::cout << *ptr1 << "\n";
        }

    } // call of UniquePtr destructor - free mem
}

struct X
{
    int a;
    double b;
    std::string s;

    auto tied() const
    {
        return std::tie(a, b, s); // -> std::tuple<int&, double&, std::string&>(a, b, s);
    }

    // bool operator==(const X& other) const
    // {
    //     return a == other.a && b == other.b && s == other.s;
    // }

    // bool operator<(const X& other) const
    // {
    //     if (a == other.a)
    //     {
    //         if (b == other.b)
    //         {
    //             return s < other.s;
    //         }
            
    //         return b < other.b;
    //     }

    //     return a < other.a;
    // }

    bool operator==(const X& other) const
    {
        return tied() == other.tied();
    }

    bool operator<(const X& other) const
    {
        return tied() < other.tied();
    }
};

TEST_CASE("compare")
{
    X x1{1, 3.14, "pi"};
    X x2{1, 3.14, "pi"};

    CHECK(x1 == x2);
    CHECK(x1 < x2);
}

auto calc_stats(const std::vector<int>& data)
{
    auto [min_pos, max_pos] = std::minmax_element(data.begin(), data.end());
    double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();

    return std::tuple{*min_pos, *max_pos, avg};
}

TEST_CASE("tuples")
{
    std::tuple<int, double, std::string> tpl{42, 3.14, "pi"};

    CHECK(std::get<0>(tpl) == 42);
    CHECK(std::get<1>(tpl) == 3.14);
    CHECK(std::get<2>(tpl) == "pi");

    std::vector<int> vec = {1, 2, 3, 665, 5};

    SECTION("before C++17")
    {
        int min, max;
        double avg;
        std::tie(min, max, avg) = calc_stats(vec);

        auto stats = calc_stats(vec);
    }

    SECTION("since C++17")
    {
        auto [min, max, avg] = calc_stats(vec);
        CHECK(min == 1);
        CHECK(max == 665);
    }
}