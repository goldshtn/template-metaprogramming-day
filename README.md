### C++ Template Metaprogramming workshop

This document contains accompanying labs for the C++ metaprogramming workshop by [Sasha Goldshtein](http://blog.sashag.net) ([@goldshtn](https://twitter.com/goldshtn)), delivered at the [December 2015 SDP](http://www.seladeveloperpractice.com).

#### Lab 1: Variadic templates -- Euclidean distance on tuples

Implement the following function, that finds the Euclidean distance between two points represented by tuples of integers or floating-point values:

```
template <typename Tup1, typename Tup2>
double euclidean_distance(Tup1 const &tup1, Tup2 const &tup2);
```

Euclidean distance is defined by the following formula: `sqrt( (x[0] - y[0])^2 + ... + (x[n] - y[n])^2 )`, in other words the square root of the sum of square differences pair-wise between two points.

Usage example:

```
auto distance = euclidean_distance(make_tuple(1, 2), make_tuple(4, 6));
// distance should be sqrt(3^2 + 4^2) = 5
```

Hints:

* The inner calculation can be recursive in terms of the current tuple element index with a base case 0
* Even if the tuples have a mix of integers and floating-point elements, do all the calculations with `double`s
* Note that the `std::get<I>` function works only if the parameter `I` is a compile-time constant; you can't just write a `for` loop that goes over all the elements in the tuple :-)
* To find the size of a tuple, use the `std::tuple_size` class template

> If you're really stuck, fill in the following skeleton that contains the core of the recursive solution:
>
    template <int I> struct helper {
      template <typename Tup1, typename Tup2>
      static double sum_squared_deltas(Tup1 const& tup1, Tup2 const& tup2) { ... }
    };
    template <> struct helper<0> {
      template <typename Tup1, typename Tup2>
      static double sum_squared_deltas(Tup1 const& tup1, Tup2 const& tup2) { ... }
    };

#### Lab 2: Compile-time computation -- `get<T>` for `tuple`s

C++ 14 introduces a very useful function for working with tuples: `get<T>`. Here’s an example of how it works. Note that if the type doesn’t appear exactly once in the tuple, the result should be a compilation error (not a runtime exception).

```
auto tup = std::make_tuple(4.0, "hello", 52);
std::get<double>(tup) = 3.0;
std::cout << std::get<int>(tup) << "\n";
```

If you already have a standard library that supports C++ 14, good for you, but we’re still going to implement this facility ourselves. Here is the general sketch of a possible solution:

* Build a class template `count` that you can invoke as follows: `count<T, T1, T2, ..., Tn>::value` which returns the number of times `T` appears in the list of types that follow. This can be done by a simple specialization for `count<T>` and `count<T, Head, Tail...>`.

* Build a class template `find` that you can invoke as follows: `find<T, T1, T2, ..., Tn>::value` which returns the first index at which the type `T` appears in the list of types that follow. This can be done in a very similar way to `count`.

Now, the `get<T>` function can use these helpers to call the standard `get<I>` function:

```
template <typename T, typename... Ts>
T& get(std::tuple<Ts...>& tup) {
  static_assert(count<T, Ts...>::value == 1, "T must appear exactly once");
  return std::get<find<T, Ts...>::value>(tup);
}
```

#### Lab 3: Trait and member detection -- `operator==` constraint for linear search

We would like to assert the necessary constraint when performing a linear search (such as what the STL `find` algorithm does). Specifically, the element we're looking for must be "equatable" (using `operator==`) to the elements in the input sequence.

Starting from the following code, add a static assertion that verifies the above constraint (in other words, checks that `val` can be compared to `*first` using `operator==`).

```
template <typename It, typename T>
It linear_search(It first, It last, T const& val) {
  for (; first != last && !(val == *first); ++first)
      ;
  return first;
}
```

There is no single correct solution, because there are many options. For example, you can try using the `void_t` approach, or the `constexpr`-function-based approach, to detect whether the necessary member exists.

> Note: Visual Studio 2015 doesn't have full support for expression SFINAE yet, which means your code might fail if you use the standard `void_t` approach. Try a different compiler.

#### Lab 4: Trait and member detection -- strings aren't containers, arrays are

The solution we built for detecting containers (`is_container`) is adequate for classic STL containers like `vector` and `map`, but it has two issues:

* It thinks `string` is a container, because it has a nested `::iterator` type. Although it is technically correct, we don't really want to treat strings as containers for the purpose of serialization.

* It thinks built-in arrays (like `int[5]`) are not containers, because they don't have a nested `::iterator` type. Again, for the purpose of serialization we would actually want to treat built-in arrays as containers.

Begin from the following `is_container` detector and fix the two problems above. Test your changes by using static assertions and also by using the `dump` method to print out a string and an array, and make sure you get the desired behavior.

```
// NOTE: Your standard library might already define void_t, in which case this declaration is not required
template <typename T>
using void_t = void;

template <typename T, typename = void>
struct is_container : std::false_type {
};

template <typename T>
struct is_container<T, void_t<typename T::iterator>> : std::true_type {
};

template <typename T>
void dump(std::ostream& os, T const& val);

template <typename T>
void dump(std::ostream& os, T const& val, std::true_type) {
    os << "<<< begin container of type " << typeid(T).name() << " >>>\n";
    for (auto const& elem : val) {
        dump(os, elem);
    }
    os << "<<< end container >>>\n";
}

template <typename T>
void dump(std::ostream& os, T const& val, std::false_type) {
    os << "plain value: " << val << '\n';
}

template <typename T>
void dump(std::ostream& os, T const& val) {
    dump(os, val, std::integral_constant<bool, is_container<T>::value>{});
}
```

> Hint: specialize `is_container` for strings and arrays. Specifically, strings are instantiations of the `std::basic_string<CharT, Traits, Allocator>` template, and arrays can be detected by specializing for their type and element count.

#### Lab 5: Overload management with `enable_if`

A common pattern with library classes that want to eliminate unnecessary copies is a constructor that takes a *universal reference* (also known as *forwarding reference*). By using a universal reference and `std::forward`, rvalue objects can be moved while lvalue objects can be copied into their final destination inside the newly constructed instance. For example, consider the following `window` class, which is initialized with a toolbar:

```
class window {
    toolbar toolbar_;
public:
    template <typename Toolbar>
    window(Toolbar&& toolbar) : toolbar_{ std::forward<Toolbar>(toolbar) } {
    }
    window(window const& other) {
        /* some copy construction */
    }
    window(window&& other) {
        /* some move construction */
    }
    /* some additional methods */
};
```

Perhaps surprisingly, there is a serious problem here that is known as *shadowing*. Namely, the template constructor can shadow the copy constructor. The following code does not compile:

```
toolbar tb;
window w{ tb };
window w2{ w };

error: excess elements in struct initializer
window(Toolbar&& toolbar) : toolbar_{ std::forward<Toolbar>(toolbar) } {
                                      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```

The culprit is that the compiler chooses the template overload because it can be specialized to form a better match: the `w` expression has type `window&`, which the template constructor can match exactly, whereas the copy constructor can only match after adding the `const` qualifier.

To solve this problem, the template constructor needs to be removed from the overload set when the type is `widget`. Use `enable_if` to do so.

> Hint: use `std::is_same`.

#### Lab 6: Integer sequences

Implement a function `array_to_tuple` that takes an array and returns a tuple that contains the same elements, in the same order.

> Hint: use an `index_sequence` and apply it to the array. Specifically, create a function that takes the array and a variadic `size_t... Ix` template parameter, and then expand `arr[Ix]...`.

Implement a function `invoke` that takes a function and a tuple, and returns the result of invoking that function with the tuple's elements as arguments.

> Hint: similarly to the previous task, use an `index_sequence` and expand `std::get<Ix>(tup)...`.

Revisit Lab 1 (Euclidean distance) and implement it using `index_sequence` instead of the manual recursive solution.
