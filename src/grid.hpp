#pragma once

#include "data.hpp"
#include <math.h>

struct Vector2;
struct Vector2I;

struct Vector2I{
    int x;
    int y;

    Vector2I();
    Vector2I(int, int);

    Vector2I(const Vector2&);
    Vector2I(Vector2&&);

    void operator=(const Vector2&);
    void operator=(Vector2&&);
};

struct Vector2{
    float x;
    float y;

    Vector2();
    Vector2(float, float);
    
    Vector2(const Vector2&);
    Vector2(Vector2&&);
    
    void operator=(const Vector2&);
    void operator=(Vector2&&);
    
    Vector2(const Vector2I&);
    Vector2(Vector2I&&);
    
    void operator=(const Vector2I&);
    void operator=(Vector2I&&);
    
    float size() const noexcept;
    float length() const noexcept;
    Vector2 normalized() const;
    
    friend bool operator==(Vector2, Vector2) noexcept;
    friend bool operator!=(Vector2, Vector2) noexcept;
    friend bool operator!(Vector2) noexcept;
    explicit operator bool() const noexcept;

    friend Vector2 operator+(Vector2, Vector2) noexcept;
    friend Vector2 operator-(Vector2, Vector2) noexcept;
    friend float operator*(Vector2, Vector2) noexcept;
    friend Vector2 operator*(Vector2, float) noexcept;
    friend Vector2 operator*(float, Vector2) noexcept;
    friend Vector2 operator/(Vector2, float);
};

struct Grid{
    Vector2 offset;
    Vector2 center;

    Vector2 grid_to_real(Vector2I);
};
