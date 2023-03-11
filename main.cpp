#include <vector>
#include <array>

#include "poly_union.hpp"
#include "closed_poly_union.hpp"
#include "forwarding_poly_union.hpp"

struct base : polymorphic_movable, polymorphic_copyable
{
    virtual void greet() = 0;
    virtual void const_greet() const {}
    virtual ~base() {}
};

#include <iostream>

struct c1 : base
{
    c1()
    {
        std::cout << "c1()" << std::endl;
    }

    c1(c1 const &)
    {
        std::cout << "c1(const &)" << std::endl;
    }

    c1(c1 &&)
    {
        std::cout << "c1(&&)" << std::endl;
    }

    void greet() override
    {
        std::cout << "Hello from c1" << std::endl;
    }

    ~c1() override
    {
        std::cout << "~c1()" << std::endl;
    }

    DEFINE_POLYMORPHIC_MOVE()
    DEFINE_POLYMORPHIC_COPY()
};

struct c2 : base
{
    int dummy; 

    c2()
    {
        std::cout << "c2()" << std::endl;
    }

    c2(c2 const &)
    {
        std::cout << "c2(const &)" << std::endl;
    }

    c2(c2 &&)
    {
        std::cout << "c2(&&)" << std::endl;
    }

    void greet() override
    {
        std::cout << "Hello from c2" << std::endl;
    }

    ~c2() override
    {
        std::cout << "~c2()" << std::endl;
    }

    DEFINE_POLYMORPHIC_MOVE()
    DEFINE_POLYMORPHIC_COPY()
};

struct c3 : base
{
    int dummy1;
    int dummy2;

    c3(int d1, int d2) : dummy1(d1), dummy2(d2)
    {
        std::cout << "c3(" << d1 << " ," << d2 << ")" << std::endl;
    }

    c3(c3 const & other)
        : dummy1(other.dummy1), dummy2(other.dummy2)
    {

        std::cout << "c3(const &)" << std::endl;
    }

    c3(c3 && other)
        : dummy1(other.dummy1), dummy2(other.dummy2)
    {

        std::cout << "c3(&&)" << std::endl;
    }

    void greet() override
    {
        std::cout << "Hello from c3: " << dummy1 << ", " << dummy2 << std::endl;
    }

    void const_greet() const override
    {
        std::cout << "Const hello from c3: " << dummy1 << ", " << dummy2 << std::endl;
    }

    ~c3() override
    {
        std::cout << "~c3()" << std::endl;
    }

    DEFINE_POLYMORPHIC_MOVE()
    DEFINE_POLYMORPHIC_COPY()
};

struct c_verbose : base
{
    c_verbose()
    {
        std::cout << "c_verbose()" << std::endl;
    }

    c_verbose(c_verbose && other)
    {
        std::cout << "c_verbose(&&)" << std::endl;
    }

    c_verbose(c_verbose const & other)
    {
        std::cout << "c_verbose(const &)" << std::endl;
    }

    void greet() override
    {
        std::cout << "Hello from c_verbose: " << std::endl;
    }

    ~c_verbose() override
    {
        std::cout << "~c_verbose()" << std::endl;
    }

    DEFINE_POLYMORPHIC_MOVE()
    DEFINE_POLYMORPHIC_COPY()
};

struct c_big : base
{
    c_big * next;
    c_big * prev;
    c_big * top;
    c_big * down;
    void greet() override
    {
        std::cout << "Hello from the big one on the heap: " << sizeof(c_big) << std::endl;
    }

    DEFINE_POLYMORPHIC_MOVE()
    DEFINE_POLYMORPHIC_COPY()
};

void print_header(char const * name)
{
    std::cout << std::endl
              << name << std::endl
              << "#=================================#" << std::endl;
}

void print_sub_header(char const * name)
{
}

void demonstrate_poly_union()
{
    print_header("poly_union");
    // poly_union is an open union with maximum storage for type as it is
    // constructed in an internal buffer.

    // Demonstrate basic usage by initializing the union with different types
    // and calling a virtual method.
    {
        poly_union<base, 24> b = make_poly_union<base, 24, c1>();
        b->greet();
        
        b.emplace<c2>();
        b->greet();

        b.emplace<c3>(42, 23);
        b->greet();

    }

    // A type that does not fit the storage will lead to a compilation error.
    {
        // poly_union<base, 16> b = make_poly_union<base, 16, c_big>();
    }

    // Call the copy constructor via emplace.
    {
        poly_union<base, 24> b = c1();
        b->greet();

        b.emplace<c2>(c2());
        b->greet();
    }

    // With the help of c_verbose, display constructor and destructor calls from
    // changing the "inhabitant".
    {
        {
            std::cout << "create poly_union" << std::endl;
            poly_union<base, 16> b = c_verbose();

            std::cout << "insert_copy c1 into poly_union" << std::endl;
            b.insert_copy(c1());

            std::cout << "insert_copy c_verbose into poly_union" << std::endl;
            b.insert_copy(c_verbose());

            std::cout << "emplace c1 into poly_union" << std::endl;
            b.emplace<c1>();

            std::cout << "emplace c_verbose into poly_union" << std::endl;
            b.emplace<c_verbose>();
        }
        std::cout << "finished destroying poly_union" << std::endl;
    }


    {
        {
            std::vector<poly_union<base, 24>> vs;
            vs.emplace_back(std::type_identity<c1>{});
            vs.emplace_back(std::type_identity<c2>{});
            vs.emplace_back(std::type_identity<c3>{}, 1, 2);
            vs.emplace_back(std::type_identity<c_verbose>{});
            // will not compile
            //vs.emplace_back(std::type_identity<c_big>{});


            for (auto & v : vs)
            {
                v->greet();
            }
        }

        std::cout << "move assignable = " << std::is_move_assignable_v<poly_union<base, 24>> << std::endl;
        std::cout << "copy assignable = " << std::is_copy_assignable_v<poly_union<base, 24>> << std::endl;
        std::cout << "move constructible = " << std::is_move_constructible_v<poly_union<base, 24>> << std::endl;
        std::cout << "copy constructible = " << std::is_copy_constructible_v<poly_union<base, 24>> << std::endl;
        std::cout << "storage move assignable = " << std::is_move_assignable_v<open_storage<24, base>> << std::endl;
        std::cout << "storage copy assignable = " << std::is_copy_assignable_v<open_storage<24, base>> << std::endl;
        std::cout << "storage move constructible = " << std::is_move_constructible_v<open_storage<24, base>> << std::endl;
        std::cout << "storage copy constructible = " << std::is_copy_constructible_v<open_storage<24, base>> << std::endl;
        std::cout << std::is_nothrow_default_constructible_v<poly_union<base, 24>> << std::endl;
        typedef poly_union<base, 24> type;

        //new type[10];

        std::array<type, 2> array = std::array<type, 2>{ make_poly_union<base, 24, c1>(), make_poly_union<base, 24, c2>() };
    }

    {
        typedef basic_poly_union<base, 24, bounded_storage<24, base>, c1> poly_union_type;
        std::vector<poly_union_type> vs;
        vs.reserve(4);
        vs.emplace_back(std::type_identity<c1>{});
        vs.emplace_back(std::type_identity<c2>{});
        vs.emplace_back(std::type_identity<c3>{}, 1, 2);
        vs.emplace_back(std::type_identity<c_verbose>{});

        for (auto & v : vs)
        {
            v->greet();
        }
    }
}

void demonstrate_closed_poly_union()
{
    print_header("closed_poly_union");
    // closed_poly_union is a closed union that only permits the specified
    // subclasses to be constructed.
    typedef closed_poly_union<base, c1, c2, c3, c_verbose> closed_union_type;

    {
        closed_union_type v = make_closed_poly_union<closed_union_type, c_verbose>();
        v->greet();

        // Will not compile because c_big is not part of the union.
        // v.emplace<c_big>();
    }

    {



        closed_union_type v = std::move(c_verbose());
        //union_type v = make_closed_poly_union<c_verbose>(std::type_identity<types>{});

        //polymorphic_union<base, c1, c2> v = polymorphic_union<base, c1, c2>::create<c1>();
        // make_polymorphic_union(derived_types<c1, c2>{}, std::type_identity<base>{}, std::type_identity<c1>{});
        v->greet();
        //v.emplace(std::type_identity<c2>{});
        v.emplace<c2>();
        v->greet();

        v.emplace<c3>(42, 23);
        v->greet();

    }

    // Demonstrate const correctness.
    {
        typedef closed_poly_union<base, c1, c2, c3> closed_union_type;
        closed_union_type v = closed_union_type(std::type_identity<c3>{}, 23, 42);
        v->greet();

        closed_union_type const & r = v;

        // needs to be const
        // r->greet();
        r->const_greet();
    }

}

void demonstrate_forwarding_poly_union()
{
    print_header("forwarding_poly_union");

    // forwarding_poly_union will allocate types that do not fit the internal
    // storage on the heap.  Internally, a std::unique_ptr is constructed in
    // the storage.  For this reason, it adds a small overhead for branching
    // and a field for discrimination.
    {
        forwarding_poly_union<base, 10> v = make_forwarding_poly_union<base, 10, c_big>();
        v->greet();
        v.emplace<c2>();
        v->greet();
        v.emplace<c_big>();
        v->greet();
        v.emplace<c1>();
        v->greet();
    }
}

int main(void)
{
    demonstrate_poly_union();
    demonstrate_closed_poly_union();
    demonstrate_forwarding_poly_union();
}
