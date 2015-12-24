//
// Created by Sasha Goldshtein on 12/13/15.
//

#ifndef TMP_SOLUTIONS_H
#define TMP_SOLUTIONS_H

#include <tuple>
#include <numeric>
#include <utility>

#include "member_detection.h"

namespace solutions {

    namespace lab1_direct {

        // Another option is to use plain function overloads on integral_constant<-1>
        // vs. the general case integral_constant<I> and do a recursive call.

        template <int I>
        struct helper {
            template <typename Tup1, typename Tup2>
            static double sum_squared_deltas(Tup1 const& tup1, Tup2 const& tup2) {
                double sd = static_cast<double>(std::get<I>(tup1) - std::get<I>(tup2));
                return sd*sd + helper<I-1>::sum_squared_deltas(tup1, tup2);
            }
        };

        template <>
        struct helper<-1> {
            template <typename Tup1, typename Tup2>
            static double sum_squared_deltas(Tup1 const& tup1, Tup2 const& tup2) {
                return 0.0;
            }
        };

        template <typename Tup1, typename Tup2>
        auto euclidean_distance(Tup1 const &tup1, Tup2 const &tup2) {

            static_assert(std::tuple_size<Tup1>::value == std::tuple_size<Tup2>::value,
                          "tuples must have the same size");

            double ssd = helper<std::tuple_size<Tup1>::value-1>::sum_squared_deltas(tup1, tup2);
            return std::sqrt(ssd);
        }
    }

    namespace lab1_sequences {

        template <typename Tup1, typename Tup2, size_t... Ix>
        double sum_squared_deltas(Tup1 const& tup1, Tup2 const& tup2, std::index_sequence<Ix...>) {

            double deltas[] = { (static_cast<double>(std::get<Ix>(tup1) - std::get<Ix>(tup2)))... };
            return std::accumulate(std::begin(deltas), std::end(deltas), 0.0,
                [](double acc, double delta) { return acc + delta*delta; });

        }

        template <typename Tup1, typename Tup2>
        auto euclidean_distance(Tup1 const& tup1, Tup2 const& tup2) {

            static_assert(std::tuple_size<Tup1>::value == std::tuple_size<Tup2>::value,
                          "tuples must have the same size");

            using ix_seq_t = std::make_index_sequence<std::tuple_size<Tup1>::value>;

            double ssd = sum_squared_deltas(tup1, tup2, ix_seq_t{});
            return std::sqrt(ssd);
        }

    }

    namespace lab2 {

        template <typename...>
        struct count;

        template <typename T>
        struct count<T> {
            constexpr static size_t value = 0;
        };

        template <typename T, typename Head, typename... Tail>
        struct count<T, Head, Tail...> {
            constexpr static size_t value =
                    (std::is_same<T, Head>::value ? 1 : 0) + count<T, Tail...>::value;
        };

        template <typename...>
        struct find;

        template <typename T>
        struct find<T> {
            // No ::value field, which causes a compile-time error
        };

        template <typename T, typename... Tail>
        struct find<T, T, Tail...> {
            constexpr static size_t value = 0;
        };

        template <typename T, typename Head, typename... Tail>
        struct find<T, Head, Tail...> {
            constexpr static size_t value = 1 + find<T, Tail...>::value;
        };

        template <typename T, typename... Ts>
        T& get(std::tuple<Ts...>& tup) {
            static_assert(count<T, Ts...>::value == 1, "T must appear exactly once");
            return std::get<find<T, Ts...>::value>(tup);
        }

    }

    namespace lab3 {

        template <typename T, typename S>
        constexpr auto has_eq_operator(int) -> decltype(std::declval<T>() == std::declval<S>(), true) {
            return true;
        }

        template <typename T, typename S>
        constexpr bool has_eq_operator(...) {
            return false;
        }

        template <typename It, typename T>
        It linear_search(It first, It last, T const& val, std::true_type) {
            for (; first != last && !(val == *first); ++first)
                ;
            return first;
        }

        template <typename It, typename T>
        It linear_search(It first, It last, T const& val, std::false_type) {
            return last;
        }

        template <typename It, typename T>
        It linear_search(It first, It last, T const& val) {
            using decayed_it_t = std::decay_t<decltype(*first)>;
            constexpr bool ok = has_eq_operator<T, decayed_it_t>(0);
            static_assert(ok, "'val' must have an equality operator with elements of the sequence (==)");

            return linear_search(first, last, val, std::integral_constant<bool, ok>{});
        }

    }

    namespace lab4 {

        template <typename T>
        struct is_container_better : member_detection::is_container3<T> {
        };

        template <typename CharT, typename Traits, typename Allocator>
        struct is_container_better<std::basic_string<CharT, Traits, Allocator>> : false_t {
        };

        template <typename T, size_t N>
        struct is_container_better<T[N]> : true_t {
        };

    }

    namespace lab5 {

        class toolbar {
        };

        class window {
            toolbar toolbar_;
        public:
            template <
                    typename Toolbar,
                    typename = std::enable_if_t<
                            !std::is_same<window, std::decay_t<Toolbar>>::value
                    >
            >
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

    }

    namespace lab6 {

        template <typename T, size_t N, size_t... Ix>
        auto array_to_tuple(T const(&arr)[N], std::index_sequence<Ix...>) {
            return std::make_tuple(arr[Ix]...);
        }

        template <typename T, size_t N>
        auto array_to_tuple(T const(&arr)[N]) {
            return array_to_tuple(arr, std::make_index_sequence<N>{});
        }

        template <typename Fn, typename Tup, size_t... Ix>
        auto invoke(Fn&& fn, Tup&& tup, std::index_sequence<Ix...>) {
            return std::forward<Fn>(fn)(std::get<Ix>(tup)...);
        }

        template <typename Fn, typename Tup>
        auto invoke(Fn&& fn, Tup&& tup) {
            return invoke(std::forward<Fn>(fn), std::forward<Tup>(tup),
                          std::make_index_sequence<std::tuple_size<Tup>::value>{});
        }

    }

}

#endif //TMP_SOLUTIONS_H
