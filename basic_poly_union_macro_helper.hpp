template
    < typename Base
    , int N
    , typename StorageType
#if IS_DEFINING_DEFAULT_CONSTRUCTOR(PARAM)
    , typename DefaultConstructType
#endif
    >
#if IS_DEFINING_DEFAULT_CONSTRUCTOR(PARAM)
#endif
struct basic_poly_union
    < Base
    , N
    , StorageType
#if IS_DEFINING_DEFAULT_CONSTRUCTOR(PARAM)
    , DefaultConstructType
#else
    , void
#endif
    , IS_DEFINING_MOVE(PARAM)
    , IS_DEFINING_COPY(PARAM)
    >
{
#if IS_DEFINING_MOVE(PARAM)
    basic_poly_union(basic_poly_union &&) = default;
    basic_poly_union & operator=(basic_poly_union &&) = default;
#endif
#if IS_DEFINING_COPY(PARAM)
    basic_poly_union(basic_poly_union const &) = default;
    basic_poly_union & operator=(basic_poly_union const &) = default;
#endif

#if IS_DEFINING_DEFAULT_CONSTRUCTOR(PARAM)
    basic_poly_union()
        : storage_(std::type_identity<DefaultConstructType>{})
    {
    }
#endif

    template <typename Derived, typename... Args>
    requires std::derived_from<Derived, Base>
    basic_poly_union(std::type_identity<Derived> w, Args &&... args)
        : storage_(w, std::forward<Args>(args)...)
    {
    }

    template <typename Derived>
    requires std::derived_from<Derived, Base> && std::copy_constructible<Derived>
    basic_poly_union(Derived const & v)
        : storage_(std::type_identity<Derived>{}, v)
    {
    }

    template <typename Derived>
    requires std::derived_from<Derived, Base> &&  std::move_constructible<Derived>
    basic_poly_union(Derived && v)
        : storage_(std::type_identity<Derived>{}, std::forward<Derived>(v))
    {
    }

    template <typename Derived, typename... Args>
    requires std::derived_from<Derived, Base>
    Derived & emplace(Args &&... args)
    {
        return *storage_.template emplace<Derived>(std::forward<Args>(args)...);
    }

    // call emplace with copy constructor
    template <typename Derived>
    requires std::derived_from<Derived, Base> && std::copy_constructible<Derived>
    Derived & insert_copy(Derived const & derived)
    {
        return emplace<Derived>(derived);
    }

    Base * pointer()
    {
        return storage_.pointer();
    }

    Base const * pointer() const
    {
        return storage_.pointer();
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

    StorageType storage_;
};
