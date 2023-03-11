#ifndef POLYMORPHIC_MOVABLE_HPP
#define POLYMORPHIC_MOVABLE_HPP

struct polymorphic_movable
{
    virtual void polymorphic_move_construct_in_place(std::byte * storage) = 0;
};

template <typename T>
requires std::move_constructible<T>
void generic_move_construct_in_place(T && v, std::byte * storage)
{
    ::new (storage) std::remove_reference<T>::type(v);
}

#define DEFINE_POLYMORPHIC_MOVE() \
    void polymorphic_move_construct_in_place(std::byte * storage) override \
    { \
        generic_move_construct_in_place(*this, storage); \
    }

#define DEFINE_POLYMORPHIC_MOVE_FROM_COPY() \
    void polymorphic_move_construct_in_place(std::byte * storage) override \
    { \
        generic_copy_construct_in_place(*this, storage); \
    }


#endif
