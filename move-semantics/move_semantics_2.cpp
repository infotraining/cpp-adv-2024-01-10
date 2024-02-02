#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <functional>
#include <memory>

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
        UniquePtr() : ptr_{nullptr}
        {}

        UniquePtr(nullptr_t) : ptr_{nullptr}
        {}

        explicit UniquePtr(T* ptr)
            : ptr_{ptr}
        {
        }

        UniquePtr(const UniquePtr& otherPtr) = delete;
        UniquePtr& operator=(const UniquePtr& otherPtr) = delete;

        UniquePtr(UniquePtr&& otherPtr) noexcept : ptr_{otherPtr.ptr_}
        {            
            otherPtr.ptr_ = nullptr;
        }

        UniquePtr& operator=(UniquePtr&& otherPtr) noexcept
        {
            if(this != &otherPtr)
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
            UniquePtr<Gadget> ptr_g(new Gadget(42, "ipad"));
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
            //UniquePtr<Gadget> other_ptr = ptr_g; // ERROR
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