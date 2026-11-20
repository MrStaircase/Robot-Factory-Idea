# What the hell is Vector??

Vector is a generic class that represent mathematical vectors and handles many of the expected functions.

## Vector

`Vector<Types...> a {value...}` is the main way to inirialize such a vector. Types can be omitted in some cases.

Vectors can be assigned to a vector of different types: `Vector<int, int> a = Vector<float, float>` is correct, though not lossless.

A vector may contain a type as element as long as it satisfy the `VectorElement` concept, `float`, `int` and  `double` are all satisfying.

A scalar multiplication is also defined `v * s`, `s * v` or `v / s`. The scalar needs to satisfy the ScalarOf concept for all elements of the vector.

Vector arithmetic may be perform by `a + b` and `a - b` which returns a vector of the most common types of the addition, that is `Vector<int, int> + Vector<int, float> = Vector<int, float>`.

Additionally, Vector arithmetic assignment can be done by `a += b` and `a -= b`, the type of vector `a` is not flexible and thus might cause loss of data. Namely, `{4, 7} - {5.5, 6.5} = {-1, 0}` **!Very unexpected!**

Vector dot product is defined by `a * b` and returns the dot product.

Vector can also return its squared size and size with `squard_size()`, `squared_length()`, `size()` and `length()`. It also allows for a call of `normalized()` which will return a vector of (likely) float which length is exactly 1. There is also `normalized_keep_types()` which will return the same type of vector without changing the types. Do note `{1, 1}.normalize_keep_types() = {0, 0}` **!Use with causion!**.

Vectors are also comparable to one another with `==` and `!=` as long as their elements are comparable. `{1, 1} is equal to {1.0, 1.0}`. The unary `!` operator is true if all of the elements are 0 (actually, defaultly initialized) and `if (vector)` is true if at least one of the elements is not 0.

The access to the elements of the vector is done through `std::get<0>(vector.values)`. This is quite lengthy so `Vector2`, `Vector2F` and `Vector2I` are created.

## Vector2 and Vector2F

They are synnonyms. This is a 2 dimensional vector with two float elements accessed through `vector.x` and `vector.y`. Vector2 also provides functions such as `angle()` `angle_to(Vector2 other)` and `rotated(float angle)`. These all use an angle in radians in the direction from `{1, 0}` to `{0, 1}` and the range of `angle()` is `[0, 2 * pi]`

## Vector2I

A 2 dimensional vector of two int elements. No additional methods yet.

## How to serialize a vector

`Vector` in itself is **NOT** compatible with json serialization. But `Vector2`, `Vector2F` and `Vector2I` are. If additional `Vector` requires serialization the `to_json()` and `from_json()` funcitons must be defined. see `components.md` for further detail.