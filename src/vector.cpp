#include "vector.hpp"

inline const Vector2 Vector2::ZERO{0.0f, 0.0f};
inline const Vector2 Vector2::UP{0.0f, -1.0f};
inline const Vector2 Vector2::DOWN{0.0f, 1.0f};
inline const Vector2 Vector2::LEFT{-1.0f, 0.0f};
inline const Vector2 Vector2::RIGHT{1.0f, 0.0f};

Vector2::Vector2(const Vector2& other): Vector(other){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2::Vector2(Vector2&& other): Vector(std::move(other)){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2::Vector2(const Vector<float, float>& other): Vector(other){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2::Vector2(Vector<float, float>&& other): Vector(std::move(other)){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2& Vector2::operator=(const Vector2& other){
    Vector<float, float>::operator=(other);
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
    return *this;
};

Vector2& Vector2::operator=(Vector2&& other){
    Vector<float, float>::operator=(std::move(other));
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
    return *this;
}

float Vector2::angle_to(const Vector2& other) const noexcept
{
    const float cross_product = x * other.y - y * other.x;
    const float dot_product = x * other.x + y * other.y;

    const float angle = std::atan2(cross_product, dot_product);

    return angle < 0.0f
        ? angle + 2.0f * std::numbers::pi_v<float>
        : angle;
}

float Vector2::angle() const noexcept {
    Vector2 from{1, 0};
    return from.angle_to(*this);
}

Vector2 Vector2::rotated(const float& angle) const noexcept {
    return {x * std::cos(angle) - y * std::sin(angle),
        x * std::sin(angle) + y * std::cos(angle)};
}

Vector2I::Vector2I(const Vector2I& other): Vector(other){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2I::Vector2I(Vector2I&& other): Vector(std::move(other)){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2I::Vector2I(const Vector<int, int>& other): Vector(other){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2I::Vector2I(Vector<int, int>&& other): Vector(std::move(other)){
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
}

Vector2I& Vector2I::operator=(const Vector2I& other){
    Vector<int, int>::operator=(other);
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
    return *this;
};

Vector2I& Vector2I::operator=(Vector2I&& other){
    Vector<int, int>::operator=(std::move(other));
    x = std::get<0>(this->values);
    y = std::get<1>(this->values);
    return *this;
}