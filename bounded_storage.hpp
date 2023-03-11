#ifndef CLOSED_STORAGE_HPP
#define CLOSED_STORAGE_HPP

#include <concepts>
#include <type_traits>

#include "basic_storage.hpp"

template <typename T, int N>
concept storage_size_at_most = sizeof(T) <= N;

template <int N, storage_size_at_most<N> Base>
struct bounded_storage : basic_storage<N, Base>
{

    template <typename Derived, typename... Args>
    bounded_storage(std::type_identity<Derived> w, Args &&... args)
    {
        super::template unsafe_construct<Derived>(std::forward<Args>(args)...);
    }

    ~bounded_storage()
    {
        super::unsafe_destroy_base();
    }

    bounded_storage(bounded_storage && other) noexcept
    {
        super::move_construct_in_place_base(other.pointer());
    }

    bounded_storage(bounded_storage const & other)
    {
        super::copy_construct_in_place_base(other.pointer());
    }

    bounded_storage & operator=(bounded_storage const & other)
    {
        super::unsafe_destroy_base();
        super::copy_construct_in_place_base(other.pointer());
    }

    bounded_storage & operator=(bounded_storage && other) noexcept
    {
        super::unsafe_destroy_base();
        super::move_construct_in_place_base(other.pointer());
    }

    template <std::derived_from<Base> Derived, typename... Args>
    Derived * emplace(Args &&... args)
    {
        super::unsafe_destroy_base();
        return super::template unsafe_construct<Derived>(std::forward<Args>(args)...);
    }

    Base * pointer()
    {
        return super::unsafe_base_pointer();
    }

    Base const * pointer() const
    {
        return super::unsafe_base_pointer();
    }

    private:

    typedef basic_storage<N, Base> super;
};

#endif
