//
// Created by Sasha Goldshtein on 12/11/15.
//

#ifndef TMP_TRAITS_H
#define TMP_TRAITS_H

#include <string>
#include <cctype>

namespace traits {

    namespace detail {
        // Source: http://en.cppreference.com/w/cpp/string/char_traits
        struct ci_char_traits : public std::char_traits<char> {
            static bool eq(char c1, char c2) {
                return std::toupper(c1) == std::toupper(c2);
            }

            static bool lt(char c1, char c2) {
                return std::toupper(c1) < std::toupper(c2);
            }

            static int compare(char const* s1, char const* s2, size_t n) {
                while (n-- != 0) {
                    if (std::toupper(*s1) < std::toupper(*s2)) return -1;
                    if (std::toupper(*s1) > std::toupper(*s2)) return 1;
                    ++s1;
                    ++s2;
                }
                return 0;
            }

            static char const* find(char const* s, int n, char a) {
                auto const ua(std::toupper(a));
                while (n-- != 0) {
                    if (std::toupper(*s) == ua)
                        return s;
                    s++;
                }
                return nullptr;
            }
        };

        template <typename InIt, typename OutIt>
        OutIt copy_helper(InIt first, InIt last, OutIt out, false_t) {
            std::cout << "using slow copy_helper for OutIt = " << typeid(OutIt).name() << '\n';
            for (; first != last; ++first, ++out) {
                *out = *first;
            }
            return out;
        }

        template <typename InIt, typename OutIt>
        OutIt copy_helper(InIt first, InIt last, OutIt out, true_t) {
            std::cout << "using fast copy_helper for OutIt = " << typeid(OutIt).name() << '\n';
            size_t count = (last - first);
            std::memmove(out, first, count * sizeof(*first));
            return out + count;
        }

        template <typename InIt, typename OutIt>
        struct is_safe_to_memmove_iter {
            using decayed_in_t = std::decay_t<decltype(*val_of_t<InIt>())>;
            using decayed_out_t = std::decay_t<decltype(*val_of_t<OutIt>())>;
            using type = typename bool_t<
                    same_v<decayed_in_t, decayed_out_t> &&
                    std::is_pointer<InIt>::value &&
                    std::is_trivially_copy_assignable<decayed_in_t>::value
            >::type;
        };

        template <typename InIt, typename OutIt>
        using is_safe_to_memmove_iter_t = typename is_safe_to_memmove_iter<InIt, OutIt>::type;
    }

    typedef std::basic_string<char, detail::ci_char_traits> ci_string;

    template <typename InIt, typename OutIt>
    OutIt copy(InIt first, InIt last, OutIt out) {
        return detail::copy_helper(first, last, out,
                                   typename detail::is_safe_to_memmove_iter_t<InIt, OutIt>::type{});
    }
}

#endif //TMP_TRAITS_H
