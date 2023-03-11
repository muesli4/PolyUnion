#ifndef OPEN_STORAGE_HPP
#define OPEN_STORAGE_HPP

#include <concepts>
#include <memory>
#include <type_traits>

#include "basic_storage.hpp"

template <int N, typename Base>
struct forwarding_storage : basic_storage<N, Base>
{
    template <std::derived_from<Base> Derived, typename... Args>
    forwarding_storage(std::type_identity<Derived> w, Args &&... args)
    {
        bool constexpr new_one_fits = sizeof(Derived) <= MinimumN;

        is_forwarded_ = !new_one_fits;
        if constexpr (new_one_fits)
        {
            super::template unsafe_construct<Derived>(std::forward<Args>(args)...);
        }
        else
        {
            construct_forwarded<Derived>(std::forward<Args>(args)...);
        }
    }

    ~forwarding_storage()
    {
        destroy();
    }

    forwarding_storage(forwarding_storage && other) noexcept
    {
        if (other.is_forwarded_)
        {
            super::template unsafe_construct<unique_base_ptr>(std::move(other.unsafe_unique_ptr_reference()));
        }
        else
        {
            super::move_construct_in_place_base(other.pointer());
        }
    }

    forwarding_storage(forwarding_storage const & other)
    {
        if (other.is_forwarded_)
        {
            Base * copy = copy_construct_on_heap(other);
            super::template unsafe_construct<unique_base_ptr>(copy);
        }
        else
        {
            super::copy_construct_in_place_base(other.pointer());
        }
    }

    forwarding_storage & operator=(forwarding_storage const & other)
    {
        if (is_forwarded_ && other.is_forwarded_)
        {
            unsafe_unique_ptr_reference().reset(copy_construct_on_heap(other));
        }
        else
        {
            destroy();
            super::copy_construct_in_place_base(other.pointer());
        }
    }

    forwarding_storage & operator=(forwarding_storage && other) noexcept
    {
        if (is_forwarded_ && other.is_forwarded_)
        {
            unsafe_unique_ptr_reference().reset(move_construct_on_heap(other));
        }
        else
        {
            destroy();
            super::move_construct_in_place_base(other.pointer());
        }
    }

    template <std::derived_from<Base> Derived, typename... Args>
    Derived * emplace(Args &&... args)
    {
        bool constexpr new_one_fits = sizeof(Derived) <= MinimumN;
        if (is_forwarded_)
        {
            if constexpr (new_one_fits)
            {
                is_forwarded_ = false;
                super::template unsafe_destroy<unique_base_ptr>();
                return super::template unsafe_construct<Derived>(std::forward<Args>(args)...);
            }
            else
            {
                auto & uptr = unsafe_unique_ptr_reference();
                uptr = std::make_unique<Derived>(std::forward<Args>(args)...);
                return reinterpret_cast<Derived *>(uptr.get());
            }
        }
        else
        {
            super::unsafe_destroy_base();
            if constexpr (new_one_fits)
            {
                return super::template unsafe_construct<Derived>(std::forward<Args>(args)...);
            }
            else
            {
                is_forwarded_ = true;
                return construct_forwarded<Derived>(std::forward<Args>(args)...);
            }
        }
    }

    Base * pointer()
    {
        if (is_forwarded_)
        {
            return unsafe_unique_ptr_reference().get();
        }
        else
        {
            return super::unsafe_base_pointer();
        }
    }

    Base const * pointer() const
    {
        if (is_forwarded_)
        {
            return unsafe_unique_ptr_reference().get();
        }
        else
        {
            return super::unsafe_base_pointer();
        }
    }

    private:

    typedef std::unique_ptr<Base> unique_base_ptr;

    void destroy()
    {
        if (is_forwarded_)
        {
            super::template unsafe_destroy<unique_base_ptr>();
        }
        else
        {
            super::unsafe_destroy_base();
        }
    }

    Base * copy_construct_on_heap(forwarding_storage const & other)
    {
        std::unique_ptr<std::byte[N]> heap_storage = std::make_unique<std::byte[N]>();
        super::copy_construct_in_place_base(other.unsafe_unique_ptr_reference().get(), heap_storage.get());
        return reinterpret_cast<Base *>(heap_storage.release());
    }

    Base * move_construct_on_heap(forwarding_storage && other)
    {
        std::unique_ptr<std::byte[N]> heap_storage = std::make_unique<std::byte[N]>();
        super::move_construct_in_place_base(other.unsafe_unique_ptr_reference().get(), heap_storage.get());
        return reinterpret_cast<Base *>(heap_storage.release());
    }

    unique_base_ptr & unsafe_unique_ptr_reference()
    {
        return *super::template unsafe_pointer<unique_base_ptr>();
    }


    static int constexpr MinimumN = N < sizeof(unique_base_ptr) ? sizeof(unique_base_ptr) : N;

    typedef basic_storage<N, Base> super;

    template <typename Derived, typename... Args>
    Derived * construct_forwarded(Args &&... args)
    {
        auto result = super::template unsafe_construct<unique_base_ptr>(std::make_unique<Derived>(std::forward<Args>(args)...));
        return reinterpret_cast<Derived *>(result->get());
    }

    bool is_forwarded_;
};

#endif
