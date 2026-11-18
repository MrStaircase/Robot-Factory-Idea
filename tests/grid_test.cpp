#include <catch2/catch_test_macros.hpp>

#include "grid.hpp"

TEST_CASE("Vector2 coverts to Vector2I", "[grid]"){
    Vector2 vector {10.8f, 5.6f};
    Vector2I vectori;
    SECTION("Copy"){

        SECTION("Constructor"){
            vectori = Vector2I{vector};
        }

        SECTION("Assignment"){
            vectori = vector;
        }
    
        REQUIRE(vectori.x == 10);
        REQUIRE(vectori.y == 5);
        REQUIRE(vector.x == 10.8f);
        REQUIRE(vector.y == 5.6f);
    }

    SECTION("Move"){

        SECTION("Constructor"){
            vectori = Vector2I{std::move(vector)};
        }

        SECTION("Assignment"){
            vectori = std::move(vector);
        }
    
        REQUIRE(vectori.x == 10);
        REQUIRE(vectori.y == 5);
        REQUIRE(vector.x == 0);
        REQUIRE(vector.y == 0);
    }
}

TEST_CASE("Vector2 copy/move", "[grid]"){
    Vector2 vec1 {10.8f, 5.6f};
    Vector2 vec2;

    SECTION("Copy"){

        SECTION("Constructor"){
            vec2 = Vector2{vec1};
        }

        SECTION("Assignment"){
            vec2 = vec1;
        }
    
        REQUIRE(vec2.x == 10.8f);
        REQUIRE(vec2.y == 5.6f);
        REQUIRE(vec1.x == 10.8f);
        REQUIRE(vec1.y == 5.6f);
    }

    SECTION("Move"){

        SECTION("Constructor"){
            vec2 = Vector2{std::move(vec1)};
        }

        SECTION("Assignment"){
            vec2 = std::move(vec1);
        }
    
        REQUIRE(vec2.x == 10.8f);
        REQUIRE(vec2.y == 5.6f);
        REQUIRE(vec1.x == 0);
        REQUIRE(vec1.y == 0);
    }
}

TEST_CASE("Vector2I is assignable to Vector2", "[grid]"){
    Vector2I vectori {10, 5};
    Vector2 vector;
    SECTION("Copy"){

        SECTION("Constructor"){
            vector = Vector2{vectori};
        }

        SECTION("Assignment"){
            vector = vectori;
        }
    
        REQUIRE(vector.x == 10.0f);
        REQUIRE(vector.y == 5.0f);
        REQUIRE(vectori.x == 10);
        REQUIRE(vectori.y == 5);
    }

    SECTION("Move"){

        SECTION("Constructor"){
            vector = Vector2{std::move(vectori)};
        }

        SECTION("Assignment"){
            vector = std::move(vectori);
        }
    
        REQUIRE(vector.x == 10.0f);
        REQUIRE(vector.y == 5.0f);
        REQUIRE(vectori.x == 0);
        REQUIRE(vectori.y == 0);
    }
}

TEST_CASE("Vector2 size, legnth, normalized", "[grid]"){
    Vector2 vector{3, 4};

    REQUIRE(vector.size() == 5);
    REQUIRE(vector.length() == 5);

    Vector2 normalized = vector.normalized();

    REQUIRE(normalized.x == 0.6f);
    REQUIRE(normalized.y == 0.8f);

    Vector2 zero;

    REQUIRE_THROWS_AS(zero.normalized(), std::overflow_error);
}

TEST_CASE("Vector2 equation operator", "[grid]"){
    Vector2 op1{3, 4};
    Vector2 op2{3, 4};
    Vector2 op3{2, 6};
    Vector2 op4;

    REQUIRE(op1 == op2);
    REQUIRE(!(op2 == op3));
    REQUIRE(op1 != op3);
    REQUIRE(!(op1 != op2));
    REQUIRE(!op4);
    REQUIRE(op2);
}

TEST_CASE("Vector2 math operators", "[grid]"){
    Vector2 op1{3, 4};
    Vector2 op2{1, 2};
    float op3 = 2;

    REQUIRE(op1 + op2 == Vector2{4, 6});
    REQUIRE(op1 - op2 == Vector2{2, 2});
    REQUIRE(op1 * op2 == 11);
    REQUIRE(op1 * op3 == Vector2{6, 8});
    REQUIRE(op3 * op2 == Vector2{2, 4});
    REQUIRE(op1 / op3 == Vector2{1.5, 2});
    REQUIRE_THROWS_AS(op1 / 0, std::overflow_error);
}
