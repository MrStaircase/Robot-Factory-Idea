#include "grid.hpp"

Vector2I::Vector2I() : Vector2I(0, 0) { }

Vector2I::Vector2I(int _x, int _y) : x(_x), y(_y) { }

Vector2I::Vector2I(const Vector2& vector) :
    x(static_cast<int>(vector.x)),
    y(static_cast<int>(vector.y))
{ }

Vector2I::Vector2I(Vector2&& vector) : 
    x(static_cast<int>(std::move(vector.x))),
    y(static_cast<int>(std::move(vector.y)))
{
    vector.x = 0;
    vector.y = 0;
}

void Vector2I::operator=(const Vector2& vector){
    x = static_cast<int>(vector.x);
    y = static_cast<int>(vector.y);
}

void Vector2I::operator=(Vector2&& vector){
    x = static_cast<int>(std::move(vector.x));
    y = static_cast<int>(std::move(vector.y));

    vector.x = 0;
    vector.y = 0;
}

Vector2::Vector2() : Vector2(0, 0) { }

Vector2::Vector2(float _x, float _y) : x(_x), y(_y) { }

Vector2::Vector2(const Vector2& vector) :
    x(vector.x),
    y(vector.y)
{ }

Vector2::Vector2(Vector2&& vector) : 
    x(std::move(vector.x)),
    y(std::move(vector.y))
{
    vector.x = 0;
    vector.y = 0;
}

void Vector2::operator=(const Vector2& vector){
    x = vector.x;
    y = vector.y;
}

void Vector2::operator=(Vector2&& vector){
    x = std::move(vector.x);
    y = std::move(vector.y);

    vector.x = 0;
    vector.y = 0;
}

Vector2::Vector2(const Vector2I& vector) :
    x(vector.x),
    y(vector.y)
{ }

Vector2::Vector2(Vector2I&& vector) : 
    x(std::move(vector.x)),
    y(std::move(vector.y))
{
    vector.x = 0;
    vector.y = 0;
}

void Vector2::operator=(const Vector2I& vector){
    x = vector.x;
    y = vector.y;
}

void Vector2::operator=(Vector2I&& vector){
    x = std::move(vector.x);
    y = std::move(vector.y);

    vector.x = 0;
    vector.y = 0;
}

float Vector2::size() const noexcept {
    return sqrt(x * x + y * y);
}

float Vector2::length() const noexcept {
    return sqrt(x * x + y * y);
}

Vector2 Vector2::normalized() const {
    float _size = size();
    if (_size == 0){
        throw std::overflow_error("vector is of length 0");
    }
    return {x / _size, y / _size};
}

bool operator==(Vector2 op1, Vector2 op2) noexcept {
    return op1.x == op2.x && op1.y == op2.y;
}

bool operator!=(Vector2 op1, Vector2 op2) noexcept {
    return op1.x != op2.x || op1.y != op2.y;
}

bool operator!(Vector2 op) noexcept {
    return op.x == .0f && op.y == .0f;
}

Vector2::operator bool() const noexcept {
    return x != .0f || y != .0f;
}

Vector2 operator+(Vector2 op1, Vector2 op2) noexcept {
    return {op1.x + op2.x, op1.y + op2.y};
}

Vector2 operator-(Vector2 op1, Vector2 op2) noexcept {
    return {op1.x - op2.x, op1.y - op2.y};
}

float operator*(Vector2 op1, Vector2 op2) noexcept {
    return op1.x * op2.x + op1.y * op2.y;
}

Vector2 operator*(Vector2 op1, float op2) noexcept {
    return {op1.x * op2, op1.y * op2};
}

Vector2 operator*(float op1, Vector2 op2) noexcept {
    return {op1 * op2.x, op1 * op2.y};
}

Vector2 operator/(Vector2 op1, float op2){
    if (op2 == 0){
        throw std::overflow_error("division by zero");
    }
    return {op1.x / op2, op1.y / op2};
}