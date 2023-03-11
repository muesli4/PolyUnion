# What is it?

PolyUnion implements a polymorphic union in C++20 with the following features.

* Polymorphic behavior without extra memory allocation.
* Similar to `std::variant` but with an OOP interface instead of discriminator-based visitors.
* Upper-bound memory requirement per subclass instance that is enforced at compile-time.
* High memory locality as class data is local.
* No additional pointer is necessary.

There are also some downsides.

* The amount of space wasted depends on the difference in required storage space for subclasses.
* Optional move and copy constructors require modification of class hierarchy.

# Types

* `poly_union` is an open union with bounded storage size.
* `closed_poly_union` is a closed union with bounded storage size.
* `forwarding_poly_union` is an open union that may use allocation for subclasses that exceed the storage size bound.
* `basic_poly_union` is the base type that enables extended configuration.  See the documentation for more details.
* `basic_storage`, `bounded_storage`, and `forwarding_storage` provide different storage behavior.
* `polymorphic_copyable` and `polymorphic_movable` provide optional copy and move semantics.

# Example

```
std::vector<poly_union<shape, 32>> shapes;
shapes.emplace_back(std::type_identity<point>{}); // in-place construct with default constructor (e.g., origin)
shapes.emplace_back(std::type_identity<triangle>{}, p1, p2, p3);
shapes.emplace_back(std::type_identity<rectangle>{}, p1, p2, length);
shapes.emplace_back(std::type_identity<circle>{}, p4, r);
// will not compile
//shapes.emplace_back(std::type_identity<static_polygon<8>>{}, p1, p2, p3, p4, p5, p6, p7, p8);

for (auto & v : shapes)
{
    std::cout << "Shape " << v->name() << " has area " << v->area() << std::endl;
}

// replace with copy of p1
shapes[3].emplace<point>(p1);

// same as above
shapes[2].insert_copy(p1);

// replace by move constructed value
shapes[1].emplace<polygon>(std::move(other_polygon));

// optional copy or move assignment - available if shape implements polymorphic_movable or polymorphic_copyable
shapes[0] = poly_union<shape, 32>(std::type_identity<point>{});

// optional copy construction of poly_union - available if shape implements polymorphic_copyable
shapes.emplace_back(shapes[0]);
```

See [main.cpp](main.cpp) for more examples.

# Scope

This is just a proof of concept and an opportunity for me to refresh my C++ skills.

* The namespace is missing.
* Handling `noexcept` in move and copy construction is missing.
* Move and copy assignment is not optimal because the constructors are employed most of the time.
* Once static reflection and code generation becomes available, some of the macros may be replaced.
