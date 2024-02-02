#include "utils.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <map>

using Utils::Gadget;

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

namespace LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    void use(Gadget* g);
    void use_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
} // namespace LegacyCode

TEST_CASE("legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    {
        use_gadget(get_gadget("ipad"));
    } // memory leak

    {
        Gadget* g = new Gadget(13, "ipad");

        use(g);
        // use_gadget(g);  // UB!!! - use after delete
        //  std::cout << g->name() << std::endl; // UB!!! - use after delete
    }

    {
        Gadget* g = get_gadget("ipad");

        use(g);

        // delete g; // UB!!! - second delete
    }
}

namespace ModernCpp
{
    // forward declarations
    std::unique_ptr<Gadget> get_gadget(const std::string& name);
    void use(std::unique_ptr<Gadget> g);
    void use_gadget(Gadget* g);

    // definitions
    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 665;
        return std::unique_ptr<Gadget>(new Gadget(++id, name));
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
} // namespace LegacyCode

TEST_CASE("memory management - modern C++")
{
    using namespace ModernCpp;

    SECTION("one")
    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());
    }

    SECTION("two")
    {
        use_gadget(get_gadget("ipad").get());
    }

    SECTION("three")
    {
        std::unique_ptr<Gadget> g(new Gadget(13, "ipad"));

        use(std::move(g));

        g = get_gadget("smartwatch");        

        use_gadget(g.get());
    }

    SECTION("four")
    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use(std::move(g));

        use(get_gadget("phone"));        
    }
}

TEST_CASE("vector gadget")
{
    using namespace ModernCpp;

    std::vector<std::unique_ptr<Gadget>> gadgets;

    gadgets.push_back(get_gadget("ipad1"));
    gadgets.push_back(get_gadget("ipad2"));
    gadgets.push_back(get_gadget("ipad3"));

    std::unique_ptr<Gadget> ptr_g = get_gadget("ipad4");
    gadgets.push_back(std::move(ptr_g));

    for(const auto& g : gadgets)
    {
        if (g)
            use_gadget(g.get());
    }

    std::cout << "--------- before move to use-\n";

    use(std::move(gadgets[2]));

    std::cout << "--------- after move to use\n";

    gadgets.erase(gadgets.begin());

    std::cout << "---------\n";

    for(const auto& g : gadgets)
    {
        if (g)
            use_gadget(g.get());
    }
}


TEST_CASE("shared_ptrs")
{
    using namespace std;

    map<string, shared_ptr<Gadget>> gadgets;
    weak_ptr<Gadget> weak_g;

    {
        std::shared_ptr<Gadget> g1 = std::make_shared<Gadget>(42, "ipad");
        std::shared_ptr<Gadget> g2 = g1;
        weak_g = g1;
        CHECK(g1.use_count() == 2);        

        gadgets.emplace("ipad", g1);
        CHECK(g1.use_count() == 3);
    }

    CHECK(gadgets["ipad"]->id() == 42);

    gadgets.clear(); // now ipad is deleted

    std::shared_ptr<Gadget> g3 = weak_g.lock();
    if (g3) // if g3 is alive
        std::cout << g3->name() << " is alive\n";
}