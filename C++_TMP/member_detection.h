//
// Created by Sasha Goldshtein on 12/13/15.
//

#ifndef TMP_MEMBER_DETECTION_H
#define TMP_MEMBER_DETECTION_H

#include <vector>
#include <string>

namespace member_detection {

    namespace detail {

        template <typename T>
        using void_t = void;

        template <typename T, typename = void>
        struct is_container3_helper : false_t {
        };

        template <typename T>
        struct is_container3_helper<T, void_t<typename T::iterator>> : true_t {
        };

        template <typename T>
        constexpr auto is_container4_helper(int) -> decltype(val_of_t<typename T::iterator>(), true) {
            return true;
        }

        template <typename T>
        constexpr auto is_container4_helper(...) {
            return false;
        }

        template <typename T>
        void dump(std::ostream& os, T const& val);

        template <typename T>
        void dump(std::ostream& os, T const& val, true_t) {
            os << "<<< begin container of type " << typeid(T).name() << " >>>\n";
            for (auto const& elem : val) {
                dump(os, elem);
            }
            os << "<<< end container >>>\n";
        }

        template <typename T>
        void dump(std::ostream& os, T const& val, false_t) {
            os << "plain value: " << val << '\n';
        }

        template <typename T>
        void dump(std::ostream& os, T const& val) {
            dump(os, val, bool_t<is_container3_helper<T>::value>{});
        }

    }

    template <typename T>
    struct is_container1 {

        // Two types whose sizes are guaranteed to differ according to The Standard
        using when_false_t = short;
        using when_true_t = long;

        template <typename S> static when_false_t test(...);
        template <typename S> static when_true_t test(typename S::iterator*);

        constexpr static bool value = sizeof(test<T>(nullptr)) == sizeof(when_true_t);

    };

    template <typename T>
    struct is_container2 {

        template <typename S, typename = typename S::iterator>
        static std::true_type test(S&&);
        static std::false_type test(...);

        constexpr static bool value = decltype(test(val_of_t<T>()))::value;

    };

    template <typename T>
    struct is_container3 : detail::is_container3_helper<T> {
    };

    template <typename T>
    using is_container4 = bool_t<detail::is_container4_helper<T>(0)>;

    template <template <typename> class Detector>
    void test() {
        static_assert(!Detector<int>::value, "");
        static_assert(Detector<std::vector<int>>::value, "");
        static_assert(Detector<std::string>::value, "");
        static_assert(!Detector<float>::value, "");
    }

    template <typename T>
    void dump(std::ostream& os, T const& val) {
        detail::dump(os, val);
    }

}

#endif //TMP_MEMBER_DETECTION_H
