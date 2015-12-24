//
// Created by Sasha Goldshtein on 09/12/2015.
//

#ifndef TMP_COMPILE_TIME_COMPUTATION_H
#define TMP_COMPILE_TIME_COMPUTATION_H

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <initializer_list>

#include "common.h"
#include "variadics.h"

namespace compiletime {

    namespace detail {

        class cstr {
            char const* start_;
            size_t length_;
        public:
            template <size_t N>
            constexpr cstr(char const(&str)[N]) : start_(str), length_(N) {
            }
            constexpr bool is_valid_pattern() const {
                for (size_t i = 0; i < length_ - 1 /* null terminator */; ++i) {
                    if (start_[i] >= 'A' && start_[i] <= 'Z')
                        continue;

                    if (start_[i] >= 'a' && start_[i] <= 'z')
                        continue;

                    if (start_[i] == '*' || start_[i] == '?')
                        continue;

                    return false;
                }
                return true;
            }
            constexpr size_t count_of(char c) const {
                size_t count = 0;
                for (size_t i = 0; i < length_; ++i) {
                    if (start_[i] == c)
                        ++count;
                }
                return count;
            }
        };

        template <typename...> struct largest_t;

        template <typename T> struct largest_t<T> : is<T> {};

        template <typename T1, typename T2, typename... Rest>
        struct largest_t<T1, T2, Rest...> {
            using type = typename select_t<(sizeof(T1) < sizeof(T2)),
                    typename largest_t<T2, Rest...>::type,
                    typename largest_t<T1, Rest...>::type
                    >::type;
        };

        void largest_test() {
            static_assert(same_v<typename largest_t<int>::type, int>, "");
            static_assert(same_v<typename largest_t<int, double>::type, double>, "");
            static_assert(same_v<typename largest_t<char, long long, long>::type, long long>, "");
        }

        template <template <class> class>
        constexpr bool and_f() {
            return true;
        }

        template <template <class> class Op, typename T, typename... Types>
        constexpr bool and_f() {
            return Op<T>::value && and_f<Op, Types...>();
        };

        template <typename T> struct size_smaller_than_five { constexpr static bool value = sizeof(T) < 5; };

        void andf_test() {
            static_assert(and_f<size_smaller_than_five>(), "");
            static_assert(and_f<size_smaller_than_five, int>(), "");
            static_assert(and_f<size_smaller_than_five, int, char, char, float>(), "");
            static_assert(!and_f<size_smaller_than_five, long long, char, char, float>(), "");
        }
    }

#define FIND_FILES(pattern) \
    static_assert(compiletime::detail::cstr{ pattern }.is_valid_pattern(), "pattern contains invalid characters"); \
    compiletime::find_files(pattern);

    void find_files(std::string const& pattern) {
        if (!std::all_of(pattern.begin(), pattern.end(), [](char c) {
            return isalnum(c) || c == '*' || c == '?';
        })) {
            throw std::invalid_argument("pattern contains invalid characters");
        }
        // The actual work would go here
    }

    template <typename... Types, typename = allow_if_t<detail::and_f<integral_t, Types...>()>>
    auto varmax(Types... args) {
        using largest = typename detail::largest_t<Types...>::type;
        std::initializer_list<largest> list{ static_cast<largest>(args)... };
        return *std::max_element(list.begin(), list.end());
    }

    template <typename... Types>
    constexpr unsigned sizeof_args(Types&&...) {
        return sizeof...(Types);
    }

#define SAFE_PRINTF(format, ...) \
    static_assert(compiletime::detail::cstr(format).count_of('%') == compiletime::sizeof_args(__VA_ARGS__), \
    "number of arguments doesn't match the format string"); \
    variadics::printf(format, ##__VA_ARGS__);

}

#endif //TMP_COMPILE_TIME_COMPUTATION_H
