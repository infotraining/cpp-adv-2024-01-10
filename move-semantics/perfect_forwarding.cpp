#include "gadget.hpp"

#include <catch2/catch_test_macros.hpp>
#include <deque>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using namespace Helpers;

////////////////////////////////////////////////////////
///  PERFECT FORWARDING

void have_fun(Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
    g.use();
}

void have_fun(const Gadget& cg)
{
    puts(__PRETTY_FUNCTION__);
    cg.use();
}

void have_fun(Gadget&& g)
{
    puts(__PRETTY_FUNCTION__);
    g.use();
}

namespace WithoutPerfectForwarding
{
    void use(Gadget& g)
    {
        have_fun(g);
    }

    void use(const Gadget& g)
    {
        have_fun(g);
    }

    void use(Gadget&& g)
    {
        have_fun(std::move(g));
    }
} // namespace WithoutPerfectForwarding

namespace PerfectForwarding
{
    template <typename TGadget>
    void use(TGadget&& g)
    {
        have_fun(std::forward<TGadget>(g));
    }
}

TEST_CASE("using gadget")
{
    Gadget g{1, "g"};
    const Gadget cg{2, "const g"};

    using PerfectForwarding::use;

    use(g);
    use(cg);
    use(Gadget{3, "temporary gadget"});
}

void foo(int&& arg)
{}

template <typename T>
void bar(T&& arg) // universal reference
{}

TEST_CASE("r_value_ref vs. universal ref")
{
    foo(42);
    int x = 42;
    //foo(x);

    bar(42);
    bar(x);
}


struct Data
{
    std::vector<Gadget> gadgets;

    // void add(const Gadget& g)
    // {
    //     gadgets.push_back(g);
    // }

    // void add(Gadget&& g)
    // {
    //     gadgets.push_back(std::move(g));
    // }

    template <typename TGadget>
    void add(TGadget&& g)
    {
        gadgets.push_back(std::forward<TGadget>(g));
    }
};


TEST_CASE("using forwarding")
{
    Data data;

    Gadget g{42};
    data.add(g);

    data.add(Gadget{665});
}