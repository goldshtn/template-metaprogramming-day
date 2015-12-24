//
// Created by Sasha Goldshtein on 09/12/2015.
//

#ifndef TMP_VARIADICS_H
#define TMP_VARIADICS_H

#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <string>

namespace variadics {

    void print() {}

    template <typename T, typename... Ts>
    void print(T&& v, Ts&&... vs) {
        std::cout << v << '\n';
        print(std::forward<Ts>(vs)...);
    }

    void printf(std::string const& format) {
        for (auto c : format) {
            if (c == '%')
                throw std::logic_error("too many format specifiers provided");

            std::cout << c;
        }
    }

    template <typename T, typename... Rest>
    void printf(std::string const& format, T&& t, Rest&&... rest) {
        for (auto i = 0ull; i < format.size(); ++i) {
            if (format[i] == '%') {
                std::cout << std::forward<T>(t);
                printf(format.substr(i+1), std::forward<Rest>(rest)...);
                return;
            } else {
                std::cout << format[i];
            }
        }
        throw std::logic_error("too many parameters provided");
    }

}

#endif //TMP_VARIADICS_H
