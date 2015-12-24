#include <iostream>
#include <map>
#include <vector>
#include <list>

#include "variadics.h"
#include "compile_time_computation.h"
#include "traits.h"
#include "policies.h"
#include "member_detection.h"
#include "sequences.h"
#include "tuple_cat.h"

#include "solutions.h"

using namespace std::string_literals;

template <typename Fn>
void try_and_print_exception(Fn&& fn) {
    try {
        std::forward<Fn>(fn)();
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

void variadics_test() {
    variadics::print(42, "Hello", 3.14f);
    variadics::printf("The value of pi is % and this conference is %\n", 3.14, "SDP");
    try_and_print_exception([] { variadics::printf("Missing parameter %\n"); });
    try_and_print_exception([] { variadics::printf("Too many parameters %\n", 42, 43); });
}

void compile_time_test() {
    auto maxval = compiletime::varmax(123ull, 4, (short)511);
    static_assert(same_v<decltype(maxval), unsigned long long>, "");
    std::cout << "maxval = " << maxval << ", type = " << typeid(maxval).name() << '\n';

    compiletime::find_files("what?ver*"s);
    FIND_FILES("what?ver*");
    try_and_print_exception([] { compiletime::find_files("invalid$$$"s); });
    // FIND_FILES("invalid$$$");

    SAFE_PRINTF("x = %\n", 42);
    // SAFE_PRINTF("%", 17, 42);
}

void traits_test() {
    traits::ci_string a("hello"), b("HellO");
    std::cout << "these strings are equal: " << std::boolalpha << (a == b) << '\n';

    struct Integer {
        Integer() {}
        Integer(int) {}
    };
    std::vector<int> v1 { 1,2,3 };
    std::vector<Integer> v2(3);
    std::vector<int> v3(3);
    std::list<int> v4(3);
    int v5[] = { 1,2,3 };
    int v6[3];

    traits::copy(v1.begin(), v1.end(), v2.begin());
    traits::copy(v1.begin(), v1.end(), v3.begin());
    traits::copy(v1.begin(), v1.end(), v4.begin());
    traits::copy(std::begin(v5), std::end(v5), std::begin(v6));
}

void policies_test() {
    try_and_print_exception([] { policies::non_null_ptr<std::string> ps{ nullptr }; });
}

void member_detection_test() {
    member_detection::test<member_detection::is_container1>();
    member_detection::test<member_detection::is_container2>();
    member_detection::test<member_detection::is_container3>();
    member_detection::test<member_detection::is_container4>();

    member_detection::dump(std::cout, 42);
    member_detection::dump(std::cout, std::vector<int>{ 1,2,3 });
    member_detection::dump(std::cout, std::vector<std::vector<int>> { {1,2}, {3,4} });
}

void sequences_test() {
    using namespace std::string_literals;

    static_assert(sequences::make_seq<4>::type::length == 5, "");
    auto tup = std::make_tuple(42, "hello"s, 3.14f);

    sequences::print_tuple(tup);
}

void solutions_test() {

    std::cout << solutions::lab1_direct::euclidean_distance(
            std::make_tuple(1, 0, 2),
            std::make_tuple(1, 2, 3)) << '\n';

    std::cout << solutions::lab1_sequences::euclidean_distance(
            std::make_tuple(1, 0, 2),
            std::make_tuple(1, 2, 3)) << '\n';

    auto tup = std::make_tuple(4.0, "hello", 52);
    solutions::lab2::get<double>(tup) = 3.0;
    std::cout << solutions::lab2::get<int>(tup) << "\n";
    // This doesn't compile:
    // solutions::lab2::get<char>(tup);

    std::vector<int> v{ -1, 43, 5, 42, 7 };
    std::cout << *solutions::lab3::linear_search(v.begin(), v.end(), 42) << '\n';
    struct non_equatable {};
    std::vector<non_equatable> u(2);
    // This doesn't compile, which is what we wanted:
    // solutions::lab3::linear_search(u.begin(), u.end(), non_equatable{});

    static_assert(solutions::lab4::is_container_better<char[5]>::value, "");
    static_assert(!solutions::lab4::is_container_better<std::string>::value, "");
    static_assert(!solutions::lab4::is_container_better<std::wstring>::value, "");

    solutions::lab5::toolbar tb;
    solutions::lab5::window w{ tb };
    solutions::lab5::window w2{ w };

    int arr[] = { 1, 2, 3 };
    auto tup2 = solutions::lab6::array_to_tuple(arr);
    std::cout << std::get<0>(tup2) << '\n';

    auto invoker = [](int a, std::string const& b) {
        std::cout << "a = " << a << ", b = " << b << '\n';
    };
    solutions::lab6::invoke(invoker, std::make_tuple(42, "hello"));
}

int main() {
    variadics_test();
    compile_time_test();
    traits_test();
    policies_test();
    member_detection_test();
    sequences_test();
    // tupcat::tuple_cat_perf(); // commented-out because it is a bit slow

    solutions_test();

    return 0;
}