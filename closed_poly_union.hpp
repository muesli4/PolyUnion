#ifndef CLOSED_POLY_UNION_HPP
#define CLOSED_POLY_UNION_HPP

#include <concepts>

#include "bounded_storage.hpp"
#include "basic_poly_union.hpp"

template <typename Type, typename... Types>
constexpr int maximum_size_of()
{
    if constexpr (sizeof...(Types) == 0)
    {
        return sizeof(Type);
    }
    else
    {
        int constexpr remaining_size = maximum_size_of<Types...>();
        return sizeof(Type) < remaining_size ? remaining_size : sizeof(Type);
    }
}

template <bool... B>
struct bool_list
{
    static constexpr bool value_or = (B || ...);
};

template <typename T, typename... Ts>
static constexpr bool is_member = bool_list<std::is_same_v<T, Ts>...>::value_or;

/**
 * closed_poly_union is a more restricted polymorphic union type.  Only the set
 * of specified types which are subclasses of Base are permitted.  The size of
 * the union is equal to the biggest storage size of all classes.
 */
template <typename Base, std::derived_from<Base>... Derived>
struct closed_poly_union
{
    typedef basic_poly_union
        < Base
        , maximum_size_of<Base, Derived...>()
        , bounded_storage
          < maximum_size_of<Base, Derived...>()
          , Base
          >
        >
        wrapped_type;

    template <typename T, typename... Args>
    requires is_member<T, Derived...>
    closed_poly_union(std::type_identity<T> w, Args &&... args)
        : wrapped_(w, std::forward<Args>(args)...)
    {
    }

    template <typename T>
    requires is_member<T, Derived...>
    closed_poly_union(T const & v)
        : wrapped_(std::type_identity<T>{}, v)
    {
    }

    template <typename T, typename... Args>
    requires is_member<T, Derived...>
    T & emplace(Args &&... args)
    {
        return wrapped_.template emplace<T>(std::forward<Args>(args)...);
    }

    // call emplace with copy constructor
    template <typename T>
    requires is_member<T, Derived...> && std::copy_constructible<T>
    T & insert_copy(T const & v)
    {
        return emplace<T>(v);
    }

    Base * pointer()
    {
        return wrapped_.pointer();
    }

    Base const * pointer() const
    {
        return wrapped_.pointer();
    }

    Base * operator->()
    {
        return pointer();
    }

    Base const * operator->() const
    {
        return pointer();
    }

    Base & get()
    {
        return *pointer();
    }

    Base const & get() const
    {
        return *pointer();
    }

    private:

    wrapped_type wrapped_;
};

template <typename Union, typename T, typename... Args>
Union make_closed_poly_union(Args &&... args)
{
    return Union(std::type_identity<T>{}, std::forward(args)...);
}

#endif
