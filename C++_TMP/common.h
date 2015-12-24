//
// Created by Sasha Goldshtein on 09/12/2015.
//

#ifndef TMP_COMMON_H
#define TMP_COMMON_H

template <typename T>
struct is {
    using type = T;
};

template <bool B>
struct bool_t : is<bool_t<B>> {
    constexpr operator bool() const { return B; }
    constexpr static bool value = B;
};

using true_t = bool_t<true>;
using false_t = bool_t<false>;

template <typename, typename>
struct same_t : false_t {};

template <typename T>
struct same_t<T, T> : true_t {};

template <typename T1, typename T2>
constexpr bool same_v = typename same_t<T1, T2>::type{};

template <bool Condition, typename IfTrue, typename IfFalse>
struct select_t;

template <typename IfTrue, typename IfFalse>
struct select_t<true, IfTrue, IfFalse> : is<IfTrue> {};

template <typename IfTrue, typename IfFalse>
struct select_t<false, IfTrue, IfFalse> : is<IfFalse> {};

template <typename>
struct integral_t : false_t {};

template <> struct integral_t<signed char> : true_t {};
template <> struct integral_t<unsigned char> : true_t {};
template <> struct integral_t<short> : true_t {};
template <> struct integral_t<unsigned short> : true_t {};
template <> struct integral_t<int> : true_t {};
template <> struct integral_t<unsigned int> : true_t {};
template <> struct integral_t<long> : true_t {};
template <> struct integral_t<unsigned long> : true_t {};
template <> struct integral_t<long long> : true_t {};
template <> struct integral_t<unsigned long long> : true_t {};

template <bool, typename T = void>
struct allow_if_t {};

template <typename T>
struct allow_if_t<true, T> : is<T> {};

template <typename T>
T&& val_of_t();

#endif //TMP_COMMON_H
