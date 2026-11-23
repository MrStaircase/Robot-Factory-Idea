#pragma once

#include <concepts>
#include <utility>
#include <tuple>
#include <type_traits>
#include <cmath>
#include <stdexcept>
#include <numbers>
#include <cstddef>
#include <ostream>

template<typename T>
concept VectorElement = requires(
    const T& a,
    const T& b,
    float c,
    T& mutable_a,
    T&& a_r_value
){
    // binary vector-vector operations
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    
    // resetable for move operations
    { mutable_a = 0 } -> std::convertible_to<T&>;
    
    // binary vector-vector operations
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
    { a == T{} } -> std::convertible_to<bool>;
    { a != T{} } -> std::convertible_to<bool>;

    // sqrt for size calculations
    {sqrt(a)} -> std::convertible_to<float>;

    // copy-move constructor
    T(a);
    T(std::move(a_r_value));

    // copy-move assignments
    { mutable_a = b } -> std::same_as<T&>;
    { mutable_a = std::move(a_r_value) } -> std::same_as<T&>;
};

template<VectorElement... Types>
class Vector;

template<typename T>
struct IsVector : std::false_type {};

template<VectorElement... Types>
struct IsVector<Vector<Types...>> : std::true_type {};

template<typename T>
concept NotVector = !IsVector<std::remove_cvref_t<T>>::value;

template<typename T, typename U>
concept ScalarOf = NotVector<T> &&
    std::is_arithmetic_v<std::remove_cvref_t<T>> &&
    VectorElement<U> && requires(
    const T& s,
    const U& a 
){
    { a * s } -> std::convertible_to<U>;
    { s * a } -> std::convertible_to<U>;
    { a / s } -> std::convertible_to<U>;
};

template<typename T, typename U>
concept IsArithmeticAssignable = requires(
    T& t,
    const U& u 
){
    {t += u} -> std::convertible_to<T&>;
    {t -= u} -> std::convertible_to<T&>;
};

template<std::size_t N, typename T, typename... Ts>
struct RepeatedTypeImpl : RepeatedTypeImpl<N-1, T, Ts..., T>{};

template<typename T, typename... Ts>
struct RepeatedTypeImpl<0, T, Ts...> {
    using type = Vector<Ts...>;
};

template<std::size_t N, typename T>
using RepeatedType = RepeatedTypeImpl<N, T>::type;

template<VectorElement... Types>
class Vector{
public:
    using common_type = std::common_type_t<Types...>;
    using calculation_type = std::conditional_t<
        std::is_floating_point_v<common_type>,
        common_type,
        float
    >;

    template<VectorElement... OtherTypes>
    using common_calculation_type =
        std::common_type_t<
            calculation_type,
            typename Vector<OtherTypes...>::calculation_type
        >;

    static constexpr std::size_t dimensions = sizeof...(Types);


private:
    template<VectorElement... OtherTypes, std::size_t... I>
    Vector(const Vector<OtherTypes...>& other, std::index_sequence<I...>) :
        values(static_cast<Types>(std::get<I>(other.values))...) { }
    
    template<VectorElement... OtherTypes, std::size_t... I>
    Vector(Vector<OtherTypes...>&& other, std::index_sequence<I...>) :
        values(static_cast<Types>(std::move(std::get<I>(other.values)))...) {
        ((std::get<I>(other.values) = 0), ...);
    }

    template<VectorElement... OtherTypes, std::size_t... I>
    void copy_assignment_helper(const Vector<OtherTypes...>& other, std::index_sequence<I...>){
        ((std::get<I>(values) = static_cast<Types>(std::get<I>(other.values))), ...);
    }
    
    template<VectorElement... OtherTypes, std::size_t... I>
    void move_assignment_helper(Vector<OtherTypes...>&& other, std::index_sequence<I...>){
        ((std::get<I>(values) = static_cast<Types>(std::move(std::get<I>(other.values)))), ...);
        ((std::get<I>(other.values) = 0), ...);
    }

    template<
        typename Operation,
        typename Accumulator,
        VectorElement... OtherTypes,
        std::size_t... I
    >
    common_calculation_type<OtherTypes...> calculate_helper(
        const Vector<OtherTypes...>& op2,
        Operation operation,
        Accumulator accumulator,
        std::index_sequence<I...>
    ) const {
        common_calculation_type<OtherTypes...> initial = 0;

        ((initial = accumulator(
            initial,
            static_cast<common_calculation_type<OtherTypes...>>(
                operation(std::get<I>(values), std::get<I>(op2.values))
        ))), ...);

        return initial;
    }
    
    using normalized_vector = RepeatedType<dimensions, calculation_type>;
    
    template<std::size_t... I>
    normalized_vector normalized_helper(std::index_sequence<I...>) const
    {
        calculation_type _size = size();

        if (_size == 0) throw std::overflow_error("vector of length 0");

        return normalized_vector{
            (static_cast<calculation_type>(std::get<I>(values) / _size))...
        };
    }

    template<std::size_t... I>
    Vector<Types...> normalized_keep_types_helper(std::index_sequence<I...>) const
    {
        calculation_type _size = size();

        if (_size == 0) throw std::overflow_error("vector of length 0");

        return Vector<Types...>{
            (static_cast<Types>(std::get<I>(values) / _size))...
        };
    }

    template<typename Operator, std::size_t... I>
    static auto arithmetic_helper(
        const Vector<Types...>& operand,
        const Operator& op,
        std::index_sequence<I...> 
    ){
        return Vector<
            decltype(
                op.template operator()<I>(std::get<I>(operand.values))
            )...
        >{
            op.template operator()<I>(std::get<I>(operand.values))...
        };
    }

    template<typename Operator, std::size_t... I>
    void arithmetic_assignment_helper(
        const Operator& op,
        std::index_sequence<I...>
    )
    {
        (op.template operator()<I>(std::get<I>(values)), ...);
    }

    template<typename T, std::size_t... I>
    requires (std::same_as<Types, T> && ...)
    T& get_runtime(std::size_t index, std::index_sequence<I...>) {
        T* result = nullptr;

        ((index == I ? result = &std::get<I>(values) : result), ...);

        if (result == nullptr)
            throw std::out_of_range("Vector index out of range");

        return *result;
    }

    template<typename T, std::size_t... I>
    const T& get_runtime(
        std::size_t index,
        std::index_sequence<I...>
    ) const {
        const T* result = nullptr;

        ((index == I ? result = &std::get<I>(values) : result), ...);

        if (result == nullptr)
            throw std::out_of_range("Vector index out of range");

        return *result;
    }

public:
    std::tuple<Types...> values;

    Vector() : values(Types{}...) { }

    Vector(Types... args) : values(std::move(args)...) { }

    Vector(const Vector& other)
        : Vector(other, std::index_sequence_for<Types...>{})
    {}

    Vector(Vector&& other)
        : Vector(std::move(other), std::index_sequence_for<Types...>{})
    {}

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (std::constructible_from<Types, const OtherTypes&> && ...)
    )
    Vector(const Vector<OtherTypes...>& other) :
        Vector(other, std::index_sequence_for<Types...>{}) { }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (std::constructible_from<Types, OtherTypes&&> && ...)
    )
    Vector(Vector<OtherTypes...>&& other) :
        Vector(std::move(other), std::index_sequence_for<Types...>{}) { }
    
    Vector& operator=(const Vector& other)
    {
        copy_assignment_helper(
            other,
            std::index_sequence_for<Types...>{}
        );

        return *this;
    }

    Vector& operator=(Vector&& other)
    {
        move_assignment_helper(
            std::move(other),
            std::index_sequence_for<Types...>{}
        );

        return *this;
    }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (std::assignable_from<Types, const OtherTypes&> && ...)
    )
    Vector& operator=(const Vector<OtherTypes...>& other){
        copy_assignment_helper(other, std::index_sequence_for<Types...>{});
        return *this;
    }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (std::assignable_from<Types, OtherTypes&&> && ...)
    )
    Vector& operator=(Vector<OtherTypes...>&& other){
        move_assignment_helper(other, std::index_sequence_for<Types...>{});
        return *this;
    }

    template<std::size_t I>
    auto& get() {
        static_assert(I < sizeof...(Types), "Vector index out of bounds");
        return std::get<I>(values);
    }

    template<std::size_t I>
    const auto& get() const {
        static_assert(I < sizeof...(Types), "Vector index out of bounds");
        return std::get<I>(values);
    }
    
    common_type& at(std::size_t index) 
    requires (std::same_as<Types, common_type> && ...){
        return get_runtime<common_type>(index, std::index_sequence_for<Types...>{});
    }

    const common_type& at(std::size_t index) const 
    requires (std::same_as<Types, common_type> && ...){
        return get_runtime<common_type>(index, std::index_sequence_for<Types...>{});
    }

    common_type& operator[](std::size_t index) 
    requires (std::same_as<Types, common_type> && ...) {
        return get_runtime<common_type>(index, std::index_sequence_for<Types...>{});
    }

    const common_type& operator[](std::size_t index) const 
    requires (std::same_as<Types, common_type> && ...) {
        return get_runtime<common_type>(index, std::index_sequence_for<Types...>{});
    }

    calculation_type squared_size() const noexcept{
        return calculate_helper(
            *this,
            [](const auto& a, const auto& b) { return a * b; },
            [](calculation_type sum, calculation_type value)
            { return sum + value; },
            std::index_sequence_for<Types...>{}
        );;
    }

    calculation_type squared_length() const noexcept{
        return squared_size();
    }

    calculation_type size() const noexcept{
        return std::sqrt(squared_size());
    }

    calculation_type length() const noexcept{
        return size();
    }

    normalized_vector normalized() const{
        return normalized_helper(std::index_sequence_for<Types...>{});
    }

    Vector normalized_keep_types() const{
        return normalized_keep_types_helper(std::index_sequence_for<Types...>{});
    }

    template<VectorElement... OtherTypes>
    friend bool operator==(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    )
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    {
        return op1.values == op2.values;
    }

    template<VectorElement... OtherTypes>
    friend bool operator!=(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    )
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    {
        return op1.values != op2.values;
    }

    explicit operator bool() const {
        return values != std::tuple<Types...>{};
    }

    bool operator!() const {
        return !static_cast<bool>(*this);
    }

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    friend auto operator+(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    )
    {
        return arithmetic_helper(
            op1,
            [&op2]<std::size_t I>(const auto& a) {
                return a + std::get<I>(op2.values); 
            },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator+(
        const Vector<Types...>& op1,
        const S& op2
    )
    {
        return arithmetic_helper(
            op1,
            [&op2]<std::size_t I>(const auto& a){ return a + op2; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator+(
        const S& op1,
        const Vector<Types...>& op2
    )
    {
        return arithmetic_helper(
            op2,
            [&op1]<std::size_t I>(const auto& a){ return op1 + a; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    friend auto operator-(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    )
    {
        return arithmetic_helper(
            op1,
            [&op2]<std::size_t I>(const auto& a) {
                return a - std::get<I>(op2.values); 
            },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator-(
        const Vector<Types...>& op1,
        const S& op2
    )
    {
        return arithmetic_helper(
            op1,
            [&op2]<std::size_t I>(const auto& a){ return a - op2; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator-(
        const S& op1,
        const Vector<Types...>& op2
    )
    {
        return arithmetic_helper(
            op2,
            [&op1]<std::size_t I>(const auto& a){ return op1 - a; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    auto element_wise_product(const Vector<OtherTypes...>& other) const {
        return arithmetic_helper(
            *this,
            [&other]<std::size_t I>(const auto& a) {
                return a * std::get<I>(other.values); 
            },
            std::index_sequence_for<Types...>{}
        );
    }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes)
    )
    friend common_calculation_type<OtherTypes...> operator*(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    )
    {
        return op1.calculate_helper(
            op2,
            [](const auto& a, const auto& b) { return a * b; },
            [](common_calculation_type<OtherTypes...> sum, common_calculation_type<OtherTypes...> value)
            { return sum + value; },
            std::index_sequence_for<Types...>{}
        );
    }
    
    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes)
    )
    common_calculation_type<OtherTypes...> dot_product(
        const Vector<OtherTypes...>& op2
    ) const
    {
        return calculate_helper(
            op2,
            [](const auto& a, const auto& b) { return a * b; },
            [](common_calculation_type<OtherTypes...> sum, common_calculation_type<OtherTypes...> value)
            { return sum + value; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator*(
        const Vector<Types...>& op1,
        const S& op2
    )
    {
        return arithmetic_helper(
            op1,
            [&op2]<std::size_t I>(const auto& a){ return a * op2; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator*(
        const S& op1,
        const Vector<Types...>& op2
    )
    {
        return arithmetic_helper(
            op2,
            [&op1]<std::size_t I>(const auto& a){ return op1 * a; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<typename S>
    requires (
        NotVector<S> &&
        (... && ScalarOf<S, Types>)
    )
    friend auto operator/(
        const Vector<Types...>& op1,
        const S& op2
    )
    {
        if (op2 == 0) throw std::overflow_error("division by zero");
        return arithmetic_helper(
            op1,
            [&op2]<std::size_t I>(const auto& a){ return a / op2; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (IsArithmeticAssignable<Types, OtherTypes> && ...)
    )
    Vector& operator+=(
        const Vector<OtherTypes...>& other
    )
    {
        arithmetic_assignment_helper(
            [&other]<std::size_t I>(auto& a){
                a += std::get<I>(other.values);
            },
            std::index_sequence_for<Types...>{}
        );

        return *this;
    }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (IsArithmeticAssignable<Types, OtherTypes> && ...)
    )
    Vector& operator+=(
        Vector<OtherTypes...>&& other
    )
    {
        arithmetic_assignment_helper(
            [&other]<std::size_t I>(auto& a){
                a += std::get<I>(other.values);
                std::get<I>(other.values) = {};
            },
            std::index_sequence_for<Types...>{}
        );

        return *this;
    }

    template<VectorElement... OtherTypes>
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (IsArithmeticAssignable<Types, OtherTypes> && ...)
    )
    Vector& operator-=(
        const Vector<OtherTypes...>& other
    )
    {
        arithmetic_assignment_helper(
            [&other]<std::size_t I>(auto& a){
                a -= std::get<I>(other.values);
            },
            std::index_sequence_for<Types...>{}
        );

        return *this;
    }

    template<VectorElement... OtherTypes>
    Vector& operator-=(
        Vector<OtherTypes...>&& other
    )
    requires (
        sizeof...(Types) == sizeof...(OtherTypes) &&
        (IsArithmeticAssignable<Types, OtherTypes> && ...)
    )
    {
        arithmetic_assignment_helper(
            [&other]<std::size_t I>(auto& a){
                a -= std::get<I>(other.values);
                std::get<I>(other.values) = {};
            },
            std::index_sequence_for<Types...>{}
        );

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector<Types...>& vector) {
        os << "(";

        [&]<std::size_t... I>(std::index_sequence<I...>) {
            ((os << (I == 0 ? "" : ", ")
                << vector.template get<I>()), ...);
        }(std::index_sequence_for<Types...>{});

        os << ")";
        return os;
    }

    static Vector zero(){
        return Vector{};
    }

    template<VectorElement... OtherTypes, typename T>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    auto lerp(const Vector<OtherTypes...>& other, const T& t) const
    {
        return *this + (other - *this) * t;
    }

    struct VectorHash {
        std::size_t operator()(const Vector<Types...>& vector) const noexcept {
            std::size_t seed = 0;

            [&]<std::size_t... I>(std::index_sequence<I...>) {
                (
                    (seed ^= std::hash<Types>{}(vector.template get<I>())
                        + 0x9e3779b9
                        + (seed << 6)
                        + (seed >> 2)),
                    ...
                );
            }(std::index_sequence_for<Types...>{});

            return seed;
        }
    };

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    common_calculation_type<OtherTypes...> manhattan_distance(
        const Vector<OtherTypes...>& op2
    ) const {
        return calculate_helper(
            op2,
            [](const auto& a, const auto& b) { return std::abs(a - b); },
            [](common_calculation_type<OtherTypes...> sum,
                common_calculation_type<OtherTypes...> value)
                { return sum + value; },
            std::index_sequence_for<Types...>{}
        );
    }

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    common_calculation_type<OtherTypes...> chebyshev_distance(
        const Vector<OtherTypes...>& op2
    ) const {
        return calculate_helper(
            op2,
            [](const auto& a, const auto& b) { return std::abs(a - b); },
            [](common_calculation_type<OtherTypes...> maximum,
                common_calculation_type<OtherTypes...> value)
                { return std::max(maximum, value); },
            std::index_sequence_for<Types...>{}
        );
    }

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    friend common_calculation_type<OtherTypes...> manhattan_distance(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    ) {
        return op1.manhattan_distance(op2);
    }

    template<VectorElement... OtherTypes>
    requires (sizeof...(Types) == sizeof...(OtherTypes))
    friend common_calculation_type<OtherTypes...> chebyshev_distance(
        const Vector<Types...>& op1,
        const Vector<OtherTypes...>& op2
    ) {
        return op1.chebyshev_distance(op2);
    }
};

namespace std {

    template<VectorElement... Types>
    struct hash<Vector<Types...>> {
        std::size_t operator()(const Vector<Types...>& vector) const noexcept {
            typename Vector<Types...>::VectorHash hasher;
            return hasher(vector);
        }
    };

}

class Vector2 : public Vector<float, float>{
public:
    static const Vector2 ZERO;
    static const Vector2 UP;
    static const Vector2 DOWN;
    static const Vector2 LEFT;
    static const Vector2 RIGHT;
    
    using Vector<float, float>::Vector;

    float& x = std::get<0>(this->values);
    float& y = std::get<1>(this->values);

    Vector2(const Vector2&);
    Vector2(Vector2&&);

    Vector2(const Vector<float, float>&);
    Vector2(Vector<float, float>&&);

    Vector2& operator=(const Vector2&);
    Vector2& operator=(Vector2&&);

    float angle_to(const Vector2&) const noexcept;

    float angle() const noexcept;

    Vector2 rotated(const float&) const noexcept;
};

namespace std {
    template<>
    struct hash<Vector2> {
        std::size_t operator()(const Vector2& vector) const noexcept {
            typename Vector<float, float>::VectorHash hasher;
            return hasher(vector);
        }
    };
}

template<>
struct IsVector<Vector2> : std::true_type {};

using Vector2F = Vector2;

class Vector2I: public Vector<int, int>{
public:
    using Vector<int, int>::Vector;

    int& x = std::get<0>(this->values);
    int& y = std::get<1>(this->values);

    Vector2I(const Vector2I&);
    Vector2I(Vector2I&&);

    Vector2I(const Vector<int, int>&);
    Vector2I(Vector<int, int>&&);

    Vector2I& operator=(const Vector2I&);
    Vector2I& operator=(Vector2I&&);
};

template<>
struct IsVector<Vector2I> : std::true_type {};

namespace std {
    template<>
    struct hash<Vector2I> {
        std::size_t operator()(const Vector2I& vector) const noexcept {
            typename Vector<int, int>::VectorHash hasher;
            return hasher(vector);
        }
    };
}