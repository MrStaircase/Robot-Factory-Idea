#pragma once

#include "vector.hpp"
#include <vector>

template<VectorElement... Types>
class Grid{
public:
    static constexpr std::size_t dimensions = sizeof...(Types);

    using Coordinates = Vector<Types...>;
    using Index = RepeatedType<dimensions, int>;

    Coordinates origin;
    Coordinates basis;

    Index size = Index{} - 1;

    Grid() = default;

    Grid(const Coordinates& _center, const Coordinates& _basis) :
        origin(_center), basis(_basis) { }

    Grid(const Coordinates& _center, const Coordinates& _basis, const Index& _size) :
        origin(_center), basis(_basis), size(_size) { }

    Coordinates get_global_position(const RepeatedType<dimensions, int>& position){
        return origin + basis.element_wise_product(position);
    }

    template<typename... PositionTypes>
    Index get_index(const Vector<PositionTypes...>& position) const {
        Index index;

        get_index_helper(
            position,
            index,
            std::index_sequence_for<Types...>{}
        );

        return index;
    }

    bool is_inside(const Index& position) const {
        for (std::size_t i = 0; i < dimensions; ++i) {
            const int coordinate = position[i];
            const int axis_size = size[i];
            
            if (axis_size != -1 && (coordinate < 0 || coordinate >= axis_size))
                return false;
        }

        return true;
    }

    std::vector<Index> get_cardinal_neighbors(const Index& position) const {
        std::vector<Index> neighbors;

        for (std::size_t axis = 0; axis < dimensions; ++axis) {
            Index negative = position;
            Index positive = position;

            negative[axis] -= 1;
            positive[axis] += 1;

            if (is_inside(negative))
                neighbors.push_back(negative);

            if (is_inside(positive))
                neighbors.push_back(positive);
        }

        return neighbors;
    }

    std::vector<Index> get_diagonal_neighbors(const Index& position) const {
        std::vector<Index> neighbors;

        std::size_t combinations = 1;
        for (std::size_t i = 0; i < dimensions; ++i) {
            combinations *= 3;
        }

        for (std::size_t combination = 0;
            combination < combinations;
            ++combination) {

            Index neighbor = position;
            std::size_t value = combination;

            for (std::size_t axis = 0; axis < dimensions; ++axis) {

                neighbor[axis] += value % 3 - 1;
                value /= 3;
            }

            if (neighbor != position && is_inside(neighbor)) {
                neighbors.push_back(neighbor);
            }
        }

        return neighbors;
    }

private:
    template<typename... PositionTypes, std::size_t... I>
    void get_index_helper(
        const Vector<PositionTypes...>& position,
        Index& index,
        std::index_sequence<I...>
    ) const {
        ((index[I] = static_cast<int>(
            std::floor(
                (position.template get<I>() -
                origin.template get<I>()) /
                basis.template get<I>()
            )
        )), ...);
    }

};

class Grid2 : public Grid<float, float>{
public:
    using Grid<float, float>::Grid;

    std::vector<Index> get_neighbors_4(const Index& position){
        return get_cardinal_neighbors(position);
    }

    std::vector<Index> get_neighbors_8(const Index& position){
        return get_diagonal_neighbors(position);
    }
};