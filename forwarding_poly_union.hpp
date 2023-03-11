#ifndef FORWARDING_POLY_UNION_HPP
#define FORWARDING_POLY_UNION_HPP

#include "forwarding_storage.hpp"
#include "basic_poly_union.hpp"

/**
 * forwarding_poly_union is a polymorphic union type that has a maximum
 * threshold for classes that are initialized in the contained buffer storage.
 *
 * If a class is too big for the storage it will be allocated on the heap.
 */
template <typename Base, int N>
using forwarding_poly_union = basic_poly_union<Base, N, forwarding_storage<N, Base>>;

template <typename Base, int N, typename Derived, typename... Args>
forwarding_poly_union<Base, N> make_forwarding_poly_union(Args &&... args)
{
    return forwarding_poly_union<Base, N>(std::type_identity<Derived>{}, std::forward(args)...);
}

#endif
