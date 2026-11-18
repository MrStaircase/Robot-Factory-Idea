#pragma once

#include "data.hpp"

struct Vector2;
struct Vector2I;

struct Vector2I{
    int x;
    int y;

    Vector2I(const Vector2&);
};

struct Vector2{
    float x;
    float y;

    float size();
    float length();
    Vector2 normalized();
    
    Vector2(const Vector2I&);

    friend Vector2 operator+(Vector2, Vector2);
    friend Vector2 operator-(Vector2, Vector2);
    friend Vector2 operator*(Vector2, float);
    friend Vector2 operator*(float, Vector2);
    friend Vector2 operator/(Vector2, float);
    friend Vector2 operator/(float, Vector2);
};

struct Grid{
    Vector2 offset;
    Vector2 center;

    Vector2 grid_to_real(Vector2I);
};
