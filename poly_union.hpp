#ifndef POLY_UNION_HPP
#define POLY_UNION_HPP

#include "bounded_storage.hpp"
#include "basic_poly_union.hpp"

/**
 * poly_union is a polymorphic union type that provides only the specified
 * amount of storage.  Initializing classes that require a bigger storage size
 * will fail at compile-time.
 */
template <typename Base, int N>
requires storage_size_at_most<Base, N>
using poly_union = basic_poly_union<Base, N, bounded_storage<N, Base>>;

template <typename Base, int N, typename Derived, typename... Args>
poly_union<Base, N> make_poly_union(Args &&... args)
{
    return poly_union<Base, N>(std::type_identity<Derived>{}, std::forward(args)...);
}


#endif
