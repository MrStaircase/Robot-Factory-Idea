#pragma once

#include <unordered_map>
#include <vector>
#include <functional>
#include <queue>
#include "grid.hpp"
#include "vector.hpp"


class Pathfinder{
private:
    std::unordered_map<Vector2I, float> _weights;
    const Grid2& _grid;
    
public:
    struct vector_priority{
        Vector2I vector;
        float priority;

        bool operator<(const vector_priority&) const ;
        bool operator>(const vector_priority&) const ;
    };

    static constexpr float UNPASSABLE {-1};

    Pathfinder(const Grid2&);

    Pathfinder(
        const std::unordered_map<Vector2I, float>&,
        const Grid2&
    );
    Pathfinder(
        std::unordered_map<Vector2I, float>&&,
        const Grid2&
    );

    std::vector<Vector2I> reconstruct_path(
        const std::unordered_map<Vector2I, Vector2I>&,
        Vector2I
    ) const noexcept;

    std::vector<Vector2I> a_star(
        const Vector2I&,
        const Vector2I&,
        const std::function<std::vector<Vector2I>(const Vector2I&)>&,
        const std::function<float(const Vector2I&, const Vector2I&)>&
    ) const ;

    std::vector<Vector2I> a_star_cardinal(
        const Vector2I&,
        const Vector2I&
    ) const ;

    std::vector<Vector2I> a_star_diagonal(
        const Vector2I&,
        const Vector2I&
    ) const ;
};