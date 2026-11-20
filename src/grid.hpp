#include "vector.hpp"
#include <vector>

template<VectorElement... Types>
class Grid{
public:
    static constexpr std::size_t dimensions = sizeof...(Types);

    using Coordinates = Vector<Types...>;
    using Index = RepeatedType<dimensions, int>;

    Coordinates center;
    Coordinates offset;

    Index size = Index{} - 1;

    Grid() = default;

    Grid(const Coordinates& _center, const Coordinates& _offset) :
        center(_center), offset(_offset) { }

    Grid(const Coordinates& _center, const Coordinates& _offset, const Index& _size) :
        center(_center), offset(_offset), size(_size) { }

    Coordinates get_global_position(const RepeatedType<dimensions, int>& position){
        return center + offset.element_wise_product(position);
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
            const int coordinate = position.template get<int>(i);
            const int axis_size = size.template get<int>(i);
            
            if (axis_size != -1 && (coordinate < 0 || coordinate >= axis_size))
                return false;
        }

        return true;
    }

    std::vector<Index> get_neighbors(const Index& position) const {
        std::vector<Index> neighbors;

        for (std::size_t axis = 0; axis < dimensions; ++axis) {
            Index negative = position;
            Index positive = position;

            negative.template get<int>(axis) -= 1;
            positive.template get<int>(axis) += 1;

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

                neighbor.template get<int>(axis) += value % 3 - 1;
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
        ((index.template get<int>(I) = static_cast<int>(
            std::floor(
                (position.template get<I>() -
                center.template get<I>()) /
                offset.template get<I>()
            )
        )), ...);
    }

};