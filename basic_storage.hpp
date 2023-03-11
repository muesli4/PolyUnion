#ifndef BASIC_STORAGE_HPP
#define BASIC_STORAGE_HPP

#include <utility>

#include "polymorphic_movable.hpp"
#include "polymorphic_copyable.hpp"

template <int N, typename Base>
struct basic_storage
{
    protected:

    template <typename T, typename... Args>
    T * unsafe_construct(Args &&... args)
    {
        return ::new (buffer_) T(std::forward<Args>(args)...);
    }

    template <typename U>
    U * unsafe_pointer()
    {
        return reinterpret_cast<U *>(buffer_);
    }

    template <typename U>
    U const * unsafe_pointer() const
    {
        return reinterpret_cast<U const *>(buffer_);
    }

    template <typename U>
    void unsafe_destroy()
    {
        unsafe_pointer<U>()->~U();
    }

    Base * unsafe_base_pointer()
    {
        return unsafe_pointer<Base>();
    }

    Base const * unsafe_base_pointer() const
    {
        return unsafe_pointer<Base>();
    }

    void unsafe_destroy_base()
    {
        unsafe_destroy<Base>();
    }

    void move_construct_in_place_base(Base * other)
    {
        // Type information would be necessary to determine if we could move
        // with move constructor of specific type.
        if constexpr (std::is_base_of_v<polymorphic_movable, Base>)
        {
            // Assumption: Buffer is not initialized or constructor has been called before.
            polymorphic_movable * m = reinterpret_cast<polymorphic_movable *>(other);
            m->polymorphic_move_construct_in_place(buffer_);
        }
        else
        {
            static_assert(std::is_base_of_v<polymorphic_movable, Base>,
                          "Move-construction requires that Base implements polymorphic_movable");
        }
    }

    void copy_construct_in_place_base(Base const * other, std::byte * storage)
    {
        if constexpr (std::is_base_of_v<polymorphic_copyable, Base>)
        {
            // Assumption: Buffer is not initialized or constructor has been called before.
            polymorphic_copyable const * c = reinterpret_cast<polymorphic_copyable const *>(other);
            c->polymorphic_copy_construct_in_place(storage);
        }
        else
        {
            static_assert(std::is_base_of_v<polymorphic_copyable, Base>,
                          "Copy-construction requires that Base implements polymorphic_copyable");
        }
    }

    void copy_construct_in_place_base(Base const * other)
    {
        copy_construct_in_place_base(other, buffer_);
    }

    private:

    alignas(Base) std::byte buffer_[N];
};

#endif
