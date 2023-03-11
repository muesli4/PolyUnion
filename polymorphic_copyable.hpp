#ifndef POLYMORPHIC_COPYABLE_HPP
#define POLYMORPHIC_COPYABLE_HPP

struct polymorphic_copyable
{
    virtual void polymorphic_copy_construct_in_place(std::byte * storage) const = 0;
};

template <typename T>
requires std::copy_constructible<T>
void generic_copy_construct_in_place(T const & v, std::byte * storage)
{
    ::new (storage) std::remove_reference<T>::type(v);
}

#define DEFINE_POLYMORPHIC_COPY() \
    void polymorphic_copy_construct_in_place(std::byte * storage) const override \
    { \
        generic_copy_construct_in_place(*this, storage); \
    }

#endif
