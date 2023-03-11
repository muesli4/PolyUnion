#ifndef BASIC_POLY_UNION_HPP
#define BASIC_POLY_UNION_HPP

#include <type_traits>

#include "open_storage.hpp"
#include "polymorphic_movable.hpp"
#include "polymorphic_copyable.hpp"

/**
 * basic_poly_union is a wrapper for accessing a subclass via its base pointer
 * polymorphically.  The storage type determines how the subclass is
 * represented.  By using placement new, no allocation has to take place.  All
 * existing storage types own the encapsulated sub class instance.  To use this
 * class it is recommended to instead use poly_union, closed_poly_union, or
 * forwarding_poly_union.
 *
 * The concrete type to construct is passed to the constructor including
 * arguments which are forwarded to it.  A copy constructor for subclasses also
 * exists.
 *
 * The value may be changed at any time by using emplace, which works in the
 * same manner as the constructor.
 *
 * To make use of polymorphic behavior one might use the functions pointer(),
 * get(), or operator->*.  The latter is syntactically equal to how pointers
 * and smart pointers are used.
 *
 * By default basic_poly_union is not default constructible.  However, if a
 * default-constructible type is given as a template argument such a
 * constructor will be available.
 *
 * Constructors and assignment operators for the concrete types are harder to
 * implement since the concrete type is not known.  Depending on whether the
 * base class implements polymorphic_movable or polymorphic_copyable then
 * those will also be available.  Note that the assignment operator will
 * in most instances lead to the destruction of the original object and a
 * following move or copy construction.  This is not really a restriction in
 * reality.  The abstract methods need to be implemented for every subclass.  A
 * failure to do so will cause the construction of super class instance
 * instead.  Macros are provided to define those methods semi-automatically.
 * At the moment, noexcept is not available.
 */
template <typename Base, int N, typename StorageType, typename DefaultConstructType = void, bool MoveConstructible = std::derived_from<Base, polymorphic_movable>, bool CopyConstructible = std::derived_from<Base, polymorphic_copyable>>
struct basic_poly_union
{
    // Specializations are defined through basic_poly_union_macro_helper.hpp
};

#define IS_DEFINING_MOVE(n) (n & 1) != 0
#define IS_DEFINING_COPY(n) (n & 2) != 0
#define IS_DEFINING_DEFAULT_CONSTRUCTOR(n) (n & 4) != 0

#define PARAM 0
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 1
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 2
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 3
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 4
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 5
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 6
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM
#define PARAM 7
#include "basic_poly_union_macro_helper.hpp"
#undef PARAM

#undef IS_DEFINING_MOVE
#undef IS_DEFINING_COPY
#undef IS_DEFINING_DEFAULT_CONSTRUCTOR

#endif
