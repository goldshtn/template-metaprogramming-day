//
// Created by Sasha Goldshtein on 12/13/15.
//

#ifndef TMP_SEQUENCES_H
#define TMP_SEQUENCES_H

#include <tuple>

#include "variadics.h"

namespace sequences {

    template<size_t... Ns>
    struct int_seq {
        constexpr static size_t length = sizeof...(Ns);
        using type = int_seq<Ns...>;
    };

    template <size_t, typename>
    struct push_front_seq;

    template <size_t Head, size_t... Tail>
    struct push_front_seq<Head, int_seq<Tail...>> {
        using type = int_seq<Head, Tail...>;
    };

    template <size_t, typename>
    struct push_back_seq;

    template <size_t Tail, size_t... Head>
    struct push_back_seq<Tail, int_seq<Head...>> {
        using type = int_seq<Head..., Tail>;
    };

    template <size_t N>
    struct make_seq {
        using type = typename push_back_seq<N, typename make_seq<N-1>::type>::type;
    };

    template <>
    struct make_seq<0> {
        using type = int_seq<0>;
    };

    template <size_t N>
    using make_index_seq = typename make_seq<N-1>::type;

    template <typename Tup, size_t... N>
    void print_tuple(Tup const& tup, int_seq<N...>) {
        variadics::print(std::get<N>(tup)...);
    }

    template <typename Tup>
    void print_tuple(Tup const& tup) {
        constexpr size_t size = std::tuple_size<Tup>::value;
        print_tuple(tup, typename make_seq<size-1>::type{});
    }

}

#endif //TMP_SEQUENCES_H
