#include "utils.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <queue>
#include <format>
#include <random>
#include <execution>
#include <future>
#include <thread>
#include <random>

using namespace std::literals;

using Utils::Gadget;

auto foo(int a) -> int
{
    return a * 2;
}

struct Foo
{
    int factor;

    int operator()(int a)
    {
        return a * factor;
    }
};

TEST_CASE("callables")
{
    SECTION("function")
    {
        int result = foo(42);
        CHECK(result == 84);

        int (*ptr_fun)(int) = foo;
        CHECK(ptr_fun(10) == 20);
    }

    SECTION("function objects")
    {
        Foo foo_obj{2};

        int result = foo_obj(42);
        CHECK(result == 84);

        Foo another_foo_obj{10};
        CHECK(another_foo_obj(50) == 500);

        another_foo_obj.factor = 1;
        CHECK(another_foo_obj(50) == 50);
    }

    SECTION("lambdas")
    {
        auto foo_lambda = [](int a) {
            return a * 2;
        };
        CHECK(foo_lambda(22) == 44);
    }
}

////////////////////////////////////////////////////////////////////////////
struct Human
{
    int id = 0;
    std::string name;

    void print() const
    {
        std::cout << "Person(" << id << ", '" << name << "')\n";
    }
};

template <typename T>
bool is_even(T n)
{
    return n % 2 == 0;
}

namespace Cpp20
{
    bool is_even(auto n)
    {
        return n % 2 == 0;
    }
} // namespace Cpp20

TEST_CASE("is_even")
{
    CHECK(is_even(4) == true);
    CHECK(is_even(4UL) == true);
    CHECK(is_even(4LL) == true);
    CHECK(is_even(4LL) == true);
    CHECK(is_even('a') == false);
    // CHECK(is_even(2.0) == false); // ERROR no modulo for double
}

template <typename Iterator, typename F>
Iterator my_find_if(Iterator v_begin, Iterator v_end, F predicate)
{
    while ((v_begin) != v_end)
    {
        if (predicate(*(v_begin)))
            return v_begin;
        ++v_begin;
    }

    return v_end;
}

struct IsEven
{
    bool operator()(int x) { return x % 2 == 0; }
};

struct IsEvenId
{
    bool operator()(Human h)
    {
        return is_even(h.id);
    }
};

TEST_CASE("my_find_if")
{
    SECTION("happy path")
    {
        long long vec[5] = {1, 3, 5, 4, 665};
        auto pos_even = my_find_if(vec, vec + 5, is_even<long long>);
        CHECK(*pos_even == 4);
    }

    SECTION("sad path")
    {
        int vec[5] = {1, 3, 5, 665, 7};
        auto pos_even = my_find_if(vec, vec + 5, is_even<int>);
        CHECK(pos_even == vec + 5);
    }

    SECTION("humans")
    {
        std::vector<Human> people = {Human{1, "Jan"}, Human{2, "Adam"}, Human{3, "Ewa"}};

        auto pos_even_id = my_find_if(std::begin(people), std::end(people), IsEvenId{});

        CHECK(pos_even_id->name == "Adam");

        auto pos_female = my_find_if(std::begin(people), std::end(people), [](const auto& h) { return h.name == "Ewa"; });

        CHECK(pos_female->id == 3);
    }
}

/////////////////////////////////////////////////////
// lambda explanation

struct Lambda_2435762835427835
{
    auto operator()(int a) const { return a * 2; }
};

TEST_CASE("lambda expression")
{
    auto lambda_obj = [](int a) {
        return a * 2;
    };
    CHECK(lambda_obj(4) == 8);

    SECTION("is interpreted as")
    {
        auto lambda_obj = Lambda_2435762835427835{};
        CHECK(lambda_obj(4) == 8);
    }
}

struct Lambda_7236592378417513415
{
    const int& factor;

    auto operator()(int x) const { return x * factor; }
};

TEST_CASE("closures")
{
    int factor = 10;

    SECTION("capture by value")
    {
        auto multiplier = [factor](int x) {
            return x * factor;
        };

        factor = 100;

        CHECK(multiplier(5) == 50);
    }

    SECTION("capture by ref")
    {
        auto multiplier = [&factor](int x) {
            return x * factor;
        };

        factor = 100;

        CHECK(multiplier(5) == 500);
    }
}

auto create_generator(int seed)
{
    return [seed]() mutable {
        return seed++;
    };
}

TEST_CASE("generator - mutable lambda")
{
    auto gen1 = create_generator(1);
    CHECK(gen1() == 1);
    CHECK(gen1() == 2);
    CHECK(gen1() == 3);
}

TEST_CASE("capture ref as const")
{
    int factor = 10;

    auto lambda = [&factor = std::as_const(factor)](int x) {
        return factor * x;
    };
}

struct Lambda_8542369478562378949
{
    template <typename T>
    auto operator()(T x) const
    {
        return x % 2 == 0;
    };
};

TEST_CASE("generic lambdas")
{
    auto is_even = [](auto x) {
        return x % 2 == 0;
    };
}

TEST_CASE("shortcuts for captures")
{
    SECTION("explicit capture")
    {
        int sum = 0;
        int factor = 10;

        std::vector<int> vec = {1, 2, 3};
        std::for_each(vec.begin(), vec.end(), [&sum, factor](int n) { sum += n * factor; });

        CHECK(sum == 60);
    }

    SECTION("implicit captures")
    {
        int sum = 0;
        int factor = 10;

        auto all_by_value = [=](int x) { return x * factor * sum; };

        auto all_by_ref = [&](int x) { return x * factor++ * sum++; };

        auto custom1 = [=, &sum](int x) { sum += x * factor; };
        auto custom2 = [&, factor](int x) { sum += x * factor; };
    }
}

TEST_CASE("storing callables")
{
    SECTION("function ptrs")
    {
        bool(*ptr_fun)(int) = is_even<int>;
        CHECK(ptr_fun(2) == true);

        ptr_fun = [](int n) -> bool { return n % 2; };
        CHECK(ptr_fun(3) == true);
    }

    SECTION("auto")
    {
        auto fun = [](int a, int b) { return a * b; };
        CHECK(fun(5, 2) == 10);
    }

    SECTION("std::function")
    {
        std::function<bool(int)> f_predicate;

        f_predicate = is_even<int>;
        CHECK(f_predicate(10) == true);

        f_predicate = IsEven{};
        CHECK(f_predicate(12) == true);

        int m = 2;
        f_predicate = [m](int x) { return x % m == 0; };
        CHECK(f_predicate(4) == true);
    }
}

class TaskQueue
{
public:
    using Task = std::function<void()>;
    
    std::queue<Task> q_tasks_;

    void submit(const Task& t)
    {
        q_tasks_.push(t);
    }

    void run()
    {
        while(!q_tasks_.empty())
        {
            Task task = q_tasks_.front();
            q_tasks_.pop();

            try
            {
                task(); 
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }                    
        }
    }
};

struct Printer
{
    int id;

    Printer(int id) : id{id}
    {}

    void on()
    {
        std::cout << "Printer is on\n";
    }

    void off()
    {
        std::cout << "Printer is off\n";
    }

    void register_task(TaskQueue& q_tasks)
    {
        q_tasks.submit([this] { std::cout << std::format("Task from printer {:<8}\n", id); });
    }
};

void print(const std::string& msg)
{
    std::cout << msg << " is printed\n";
}

TEST_CASE("TaskQueue")
{
    TaskQueue tq;
    Printer prn{665};
    prn.register_task(tq);

    tq.submit([]{ std::cout << "Start\n";});
    tq.submit([] { print("Message"); });
    tq.submit([&prn](){ prn.on(); });
    tq.submit([&prn](){ prn.off(); throw std::runtime_error("Stack overflow"); });
    tq.submit([]{ std::cout << "Stop\n";});

    //----------------
    tq.run();
}

TEST_CASE("algorithms")
{
    std::vector<int> vec(1'000'000);

    std::random_device rnd_seed;
    std::mt19937_64 rnd_gen(rnd_seed());
    std::uniform_int_distribution rnd_distr(0, 1'000'000);

    std::generate(vec.begin(), vec.end(), [&] { return rnd_distr(rnd_gen); });

    BENCHMARK("seq") 
    {
        std::sort(vec.begin(), vec.end());
        return std::is_sorted(vec.begin(), vec.end());
    };

    BENCHMARK("par") 
    {
        std::sort(std::execution::par, vec.begin(), vec.end());
        return std::is_sorted(std::execution::par, vec.begin(), vec.end());
    };
}

/////////////////////////////////////////////////////////////////////////////////
// async functions

int calculate(int x)
{
    std::cout << "Start of calc for " << x << std::endl;
    std::random_device rnd;
    std::uniform_int_distribution rnd_dist(3000, 7200);
    size_t time_interval = rnd_dist(rnd);
    std::this_thread::sleep_for(std::chrono::milliseconds(time_interval));
    int result = x * x;
    std::cout << "End of calc for " << x << std::endl;
    return result;
}

constexpr int calculate_size(int x)
{
    int result = x * x;
    return result;
}


TEST_CASE("sync vs. async")
{
    // SECTION("sync")
    // {
    //     auto res1 = calculate(12);
    //     auto res2 = calculate(16);
    //     auto res3 = calculate(42);
    //     auto res4 = calculate(665);

    //     std::cout << res1 << " " << res2 << " " << res3 << " " << res4 << "\n";
    // }

    SECTION("async")
    {
        std::future<int> f1 = std::async(std::launch::async, calculate, 12);       
        std::future<int> f2 = std::async(std::launch::deferred, calculate, 16);       
        std::future<int> f3 = std::async(std::launch::async, calculate, 42);       
        std::future<int> f4 = std::async(std::launch::async, [] { return calculate(665); });  

        std::this_thread::sleep_for(10s);    

        std::cout << f1.get() << " " << f2.get() << " " << f3.get() << " " << f4.get() << "\n";
    }
}