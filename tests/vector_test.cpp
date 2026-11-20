#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "vector.hpp"

TEST_CASE("Vector construction", "[Vector]"){
    Vector<int, int> a;
    static_assert(a.dimensions == 2);
    REQUIRE(std::get<0>(a.values) == 0);
    REQUIRE(std::get<1>(a.values) == 0);

    Vector<int, int> b{1, 2};
    static_assert(b.dimensions == 2);
    REQUIRE(std::get<0>(b.values) == 1);
    REQUIRE(std::get<1>(b.values) == 2);

    Vector<int, float, double> c{1, 2.5f, 4.8};
    static_assert(c.dimensions == 3);
    REQUIRE(std::get<0>(c.values) == 1);
    REQUIRE(std::get<1>(c.values) == 2.5f);
    REQUIRE(std::get<2>(c.values) == 4.8);
}

TEST_CASE("Vector copy constroctor", "[Vector]"){
    Vector<int, float> c{1, 2.5f};
    Vector<int, int> d(c);

    REQUIRE(std::get<0>(d.values) == 1);
    REQUIRE(std::get<1>(d.values) == 2);

    REQUIRE(std::get<0>(c.values) == 1);
    REQUIRE(std::get<1>(c.values) == 2.5f);
}

TEST_CASE("Vector move constroctor", "[Vector]"){
    Vector<int, float> c{1, 2.5f};
    Vector<int, int> d(std::move(c));

    REQUIRE(std::get<0>(d.values) == 1);
    REQUIRE(std::get<1>(d.values) == 2);

    REQUIRE(std::get<0>(c.values) == 0);
    REQUIRE(std::get<1>(c.values) == 0);
}

TEST_CASE("Vector copy assignment", "[Vector]"){
    Vector<int, float> c{1, 2.5f};
    Vector<int, int> d = c;

    REQUIRE(std::get<0>(d.values) == 1);
    REQUIRE(std::get<1>(d.values) == 2);

    REQUIRE(std::get<0>(c.values) == 1);
    REQUIRE(std::get<1>(c.values) == 2.5f);
}

TEST_CASE("Vector move assignment", "[Vector]"){
    Vector<int, float> c{1, 2.5f};
    Vector<int, int> d = std::move(c);

    REQUIRE(std::get<0>(d.values) == 1);
    REQUIRE(std::get<1>(d.values) == 2);

    REQUIRE(std::get<0>(c.values) == 0);
    REQUIRE(std::get<1>(c.values) == 0);
}

TEST_CASE("Vector size", "[Vector]"){
    Vector<int, int> a{3, 4};
    REQUIRE(a.squared_size() == 25);
    REQUIRE(a.squared_length() == 25);
    REQUIRE(a.size() == 5);
    REQUIRE(a.length() == 5);
}

TEST_CASE("Vector normalize", "[Vector]"){
    Vector<int, int> a{3, 4};
    Vector<int, int> b;

    Vector<float, float> normal = a.normalized();

    REQUIRE(std::get<0>(normal.values) == 0.6f);
    REQUIRE(std::get<1>(normal.values) == 0.8f);
    REQUIRE(normal.size() == 1.0f);

    REQUIRE_THROWS_AS(b.normalized(), std::overflow_error);
    
    Vector<int, int> normal_keep = a.normalized_keep_types();
    
    REQUIRE(std::get<0>(normal_keep.values) == 0);
    REQUIRE(std::get<1>(normal_keep.values) == 0);

    REQUIRE_THROWS_AS(b.normalized_keep_types(), std::overflow_error);
}

TEST_CASE("Vector compoarison", "[Vector]"){
    Vector<int, int> a{3, 4};

    Vector<float, float> b{3, 4};
    Vector<int, int> c{2, 4};

    Vector<int, int> d{0, 0}; 

    REQUIRE(a == b);
    REQUIRE(a != c);
    REQUIRE(c);
    REQUIRE(!d);
}

TEST_CASE("Vector addition", "[Vector]"){
    Vector<int, int> a{1, 2};
    Vector<int, int> b{2, 4};
    Vector<int, float> c{2, 3.5f};

    static_assert(std::is_same_v<decltype(a + b), Vector<int, int>>);
    REQUIRE(a + b == Vector{3, 6});

    static_assert(std::is_same_v<decltype(a + c), Vector<int, float>>);
    REQUIRE(a + c == Vector<int, float>{3, 5.5f});
}

TEST_CASE("Vector subtraction", "[Vector]"){
    Vector<int, int> a{1, 2};
    Vector<int, int> b{2, 4};
    Vector<int, float> c{2, 3.5f};

    static_assert(std::is_same_v<decltype(b - a), Vector<int, int>>);
    REQUIRE(b - a == Vector{1, 2});

    static_assert(std::is_same_v<decltype(c - a), Vector<int, float>>);
    REQUIRE(c - a == Vector<int, float>{1, 1.5f});
}

TEST_CASE("Vector dot product", "[Vector]"){
    Vector a{1, 2};
    Vector b{2, 4};

    REQUIRE(a * b == 10);

    REQUIRE(a * a == a.squared_size());
}

TEST_CASE("Vector scalar addition", "[Vector]"){
    Vector a{1, 2};
    int b{2};
    float c(3.5f);

    REQUIRE(a + b == Vector{3, 4});
    REQUIRE(b + a == a + b);
    REQUIRE(a + c == Vector{4.5f, 5.5f});
    REQUIRE(c + a == a + c);
}

TEST_CASE("Vector scalar subtraction", "[Vector]"){
    Vector a{1, 2};
    int b{2};
    float c(3.5f);

    REQUIRE(a - b == Vector{-1, 0});
    REQUIRE(a - c == Vector{-2.5f, -1.5f});
}

TEST_CASE("Vector scalar product", "[Vector]"){
    Vector a{1, 2};
    int b{2};
    float c(3.5f);

    REQUIRE(a * b == Vector{2, 4});
    REQUIRE(b * a == a * b);
    REQUIRE(a * c == Vector{3.5f, 7.0f});
    REQUIRE(c * a == a * c);
}

TEST_CASE("Vector scalar division", "[Vector]"){
    Vector a{3, 6};
    int b{3};
    float c{1.5f};

    REQUIRE(a / b == Vector{1, 2});
    REQUIRE(a / c == Vector{2.0f, 4.0f});
    REQUIRE_THROWS_AS(a / 0, std::overflow_error);
}

TEST_CASE("Vector addition assign", "[Vector]"){
    Vector a{3, 6};
    Vector b{4, 7};
    Vector c(5.5f, 6.5f);

    a += b;
    REQUIRE(a == Vector{7, 13});
    
    b += c;
    REQUIRE(b == Vector{9, 13});

    c += a;
    REQUIRE(c== Vector{12.5f, 19.5f});
}

TEST_CASE("Vector addition assign move", "[Vector]"){
    Vector a{3, 6};
    Vector b{4, 7};

    a += std::move(b);
    REQUIRE(a == Vector{7, 13});
    REQUIRE(!b);
}

TEST_CASE("Vector subtraction assign", "[Vector]"){
    Vector a{3, 6};
    Vector b{4, 7};
    Vector c(5.5f, 6.5f);

    a -= b;
    REQUIRE(a == Vector{-1, -1});
    
    b -= c;
    REQUIRE(b == Vector{-1, 0});

    c -= a;
    REQUIRE(c== Vector{6.5f, 7.5f});
}

TEST_CASE("Vector subtraction assign move", "[Vector]"){
    Vector a{3, 6};
    Vector b{4, 7};

    a -= std::move(b);
    REQUIRE(a == Vector{-1, -1});
}

TEST_CASE("Vector zero function", "[Vector]"){
    Vector<int, int> a = Vector<int, int>::zero(); 

    REQUIRE(!a);
}

TEST_CASE("Vector lerp", "[Vector]"){
    Vector a = {1, 1, -1};
    Vector b = {1, 1, 1};
    Vector c = {1, -1, 1};

    REQUIRE(a.lerp(b, 0.5) == Vector{1, 1, 0});
    REQUIRE(a.lerp(c, 0.5) == Vector{1, 0, 0});
    REQUIRE(a.lerp(b, 0.25) == Vector{1, 1, -0.5f});
}

TEST_CASE("Vector2 initialiers", "[Vector]"){
    Vector2 a{3.2f, 5.4f};

    REQUIRE(a == Vector{3.2f, 5.4f});
    REQUIRE(a.x == 3.2f);
    REQUIRE(a.y == 5.4f);

    Vector2 b = a;
    REQUIRE(a == Vector{3.2f, 5.4f});
    REQUIRE(b == Vector{3.2f, 5.4f});

    Vector2 c = std::move(a);
    REQUIRE(c == Vector{3.2f, 5.4f});
    REQUIRE(a == Vector{0.0f, 0.0f});

    REQUIRE(c * 2 == Vector{6.4f, 10.8f});

    REQUIRE(!a);
    REQUIRE(b + c == Vector{6.4f, 10.8f});

    b += c;
    REQUIRE(b == Vector{6.4f, 10.8f});
    REQUIRE(c == Vector{3.2f, 5.4f});
    
    a += std::move(c);
    REQUIRE(a == Vector{3.2f, 5.4f});
    REQUIRE(c == Vector{0.0f, 0.0f});

    c.x = 6.0f;
    c.y += 7.0f;
    REQUIRE(c == Vector{6.0f, 7.0f});

    Vector2 d;

    d.x = 6.0f;
    d.y += 7.0f;
    REQUIRE(d == Vector{6.0f, 7.0f});
}

TEST_CASE("Vector2 angle, rotated", "[Vector]"){
    const float PI = std::numbers::pi_v<float>;
    const float HALF_PI = PI / 2.0f;
    const float TWO_PI = 2.0f * PI;
    const float EPSILON = 0.0001f;

    SECTION("angle")
    {
        REQUIRE(Vector2{1.0f, 0.0f}.angle() == Catch::Approx(0.0f));
        REQUIRE(Vector2{0.0f, 1.0f}.angle() == Catch::Approx(HALF_PI));
        REQUIRE(Vector2{-1.0f, 0.0f}.angle() == Catch::Approx(PI));
        REQUIRE(Vector2{0.0f, -1.0f}.angle() == Catch::Approx(3 * HALF_PI));

        REQUIRE(Vector2{1.0f, 1.0f}.angle() == Catch::Approx(PI / 4.0f));
        REQUIRE(Vector2{-1.0f, 1.0f}.angle() == Catch::Approx(3.0f * PI / 4.0f));
    }

    SECTION("angle_to")
    {
        const Vector2 RIGHT{1.0f, 0.0f};

        REQUIRE(RIGHT.angle_to(Vector2{1.0f, 0.0f}) == Catch::Approx(0.0f));
        REQUIRE(RIGHT.angle_to(Vector2{0.0f, 1.0f}) == Catch::Approx(HALF_PI));
        REQUIRE(RIGHT.angle_to(Vector2{-1.0f, 0.0f}) == Catch::Approx(PI));
        REQUIRE(RIGHT.angle_to(Vector2{0.0f, -1.0f}) == Catch::Approx(3.0f * HALF_PI));

        REQUIRE(Vector2{0.0f, 1.0f}.angle_to(RIGHT) == Catch::Approx(3.0f * HALF_PI));
        REQUIRE(Vector2{0.0f, -1.0f}.angle_to(RIGHT) == Catch::Approx(HALF_PI));
    }

    SECTION("rotated")
    {
        const Vector2 RIGHT{1.0f, 0.0f};

        const Vector2 up = RIGHT.rotated(HALF_PI);
        REQUIRE(up.x == Catch::Approx(0.0f).margin(EPSILON));
        REQUIRE(up.y == Catch::Approx(1.0f).margin(EPSILON));

        const Vector2 left = RIGHT.rotated(PI);
        REQUIRE(left.x == Catch::Approx(-1.0f).margin(EPSILON));
        REQUIRE(left.y == Catch::Approx(0.0f).margin(EPSILON));

        const Vector2 down = RIGHT.rotated(3.0f * HALF_PI);
        REQUIRE(down.x == Catch::Approx(0.0f).margin(EPSILON));
        REQUIRE(down.y == Catch::Approx(-1.0f).margin(EPSILON));

        const Vector2 full_rotation = RIGHT.rotated(TWO_PI);
        REQUIRE(full_rotation.x == Catch::Approx(1.0f).margin(EPSILON));
        REQUIRE(full_rotation.y == Catch::Approx(0.0f).margin(EPSILON));

        // Rotating should not modify the original vector.
        REQUIRE(RIGHT.x == Catch::Approx(1.0f));
        REQUIRE(RIGHT.y == Catch::Approx(0.0f));
    }
}

TEST_CASE("Vector element eise product", "[Vector]"){
    Vector a{1, 3};
    Vector b{3, 4.5f};
    
    REQUIRE(a.element_wise_product(b) == Vector{3, 13.5f});
}

TEST_CASE("Vector accesor", "[Vector]"){
    Vector a{1, 3};

    REQUIRE(a.get<1>() == 3);
    REQUIRE(a.get<int>(0) == 1);
}