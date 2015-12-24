//
// Created by Sasha Goldshtein on 12/13/15.
//

#ifndef TMP_POLICIES_H
#define TMP_POLICIES_H

#include <stdexcept>

namespace policies {

    template <typename T, typename SafetyPolicy>
    class smart_ptr {
        T* ptr_;
    public:
        smart_ptr(T* ptr) : ptr_{ ptr } {
            SafetyPolicy::test(ptr);
        }

        T& operator*() {
            SafetyPolicy::test(ptr_);
            return *ptr_;
        }

        T const& operator*() const {
            SafetyPolicy::test(ptr_);
            return *ptr_;
        }

        void reset(T* ptr) {
            SafetyPolicy::test(ptr_);
            ptr_ = ptr;
        }

        ~smart_ptr() {
            delete ptr_;
        }
    };

    struct unsafe_policy {
        static void test(void*) {}
    };

    struct non_null_policy {
        static void test(void* ptr) {
            if (ptr == nullptr) {
                throw std::invalid_argument("pointer was null");
            }
        }
    };

    template <typename T>
    using unsafe_ptr = smart_ptr<T, unsafe_policy>;

    template <typename T>
    using non_null_ptr = smart_ptr<T, non_null_policy>;

}

#endif //TMP_POLICIES_H
