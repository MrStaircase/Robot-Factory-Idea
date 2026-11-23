#include "path_find.hpp"

#include <iostream>

bool Pathfinder::vector_priority::operator<(const vector_priority& other) const{
    return priority < other.priority;
}

bool Pathfinder::vector_priority::operator>(const vector_priority& other) const{
    return priority > other.priority;
}

Pathfinder::Pathfinder(const Grid2& __grid) : _grid(__grid) { }

Pathfinder::Pathfinder(
    const std::unordered_map<Vector2I, float>& __weights,
    const Grid2& __grid
) : _weights(__weights), _grid(__grid) { }

Pathfinder::Pathfinder(
    std::unordered_map<Vector2I, float>&& __weights,
    const Grid2& __grid 
) : _weights(std::move(__weights)), _grid(__grid) { }

std::vector<Vector2I> Pathfinder::reconstruct_path(
    const std::unordered_map<Vector2I, Vector2I>& came_from,
    Vector2I current
) const noexcept{
    std::vector<Vector2I> return_value{current};
    
    while(came_from.contains(current)){
        current = came_from.at(current);
        return_value.emplace_back(current);
    }

    return return_value;
}

std::vector<Vector2I> Pathfinder::a_star(
    const Vector2I& start,
    const Vector2I& goal,
    const std::function<std::vector<Vector2I>(const Vector2I&)>& _neighbors,
    const std::function<float(const Vector2I&, const Vector2I&)>& _heu
) const {
    std::function<float(const Vector2I)> heu =
        std::bind(_heu, std::placeholders::_1, goal);

    std::priority_queue<vector_priority,
        std::vector<vector_priority>,
        std::greater<vector_priority>
    > open_set;

    open_set.push(vector_priority{start, heu(start)});

    std::unordered_map<Vector2I, Vector2I> came_from;

    std::unordered_map<Vector2I, float> g_score;

    g_score.emplace(start, 0);

    std::unordered_map<Vector2I, float> f_score;

    f_score.emplace(start, heu(start));

    while (!open_set.empty()){
        Vector2I current = open_set.top().vector;

        if (current == goal){
            return reconstruct_path(came_from, goal);
        }
        
        open_set.pop();

        std::vector<Vector2I> neighbors = _neighbors(current);

        for (const Vector2I& neighbor : neighbors){
            float weight = 1;

            if (_weights.contains(current)){
                weight = _weights.at(current);
                if (weight == UNPASSABLE) continue;
            }

            float tentative_g_score = g_score.at(current) + weight;

            bool is_better_g_score = true;

            if (g_score.contains(neighbor)){
                is_better_g_score = tentative_g_score < g_score.at(neighbor);
            }

            if (is_better_g_score){
                came_from.insert_or_assign(neighbor, current);
                g_score.insert_or_assign(neighbor, tentative_g_score);

                float tentative_f_score = tentative_g_score + heu(neighbor);
                f_score.insert_or_assign(neighbor, tentative_g_score + heu(neighbor));
                
                open_set.push(vector_priority{neighbor, tentative_f_score});
            }
        }
    }

    throw std::out_of_range("path not found");
}

std::vector<Vector2I> Pathfinder::a_star_cardinal(
    const Vector2I& start,
    const Vector2I& goal
) const {
    return a_star(
        start, goal,
        [this](const Vector2I& vec){ return _grid.get_cardinal_neighbors(vec); },
        [](const Vector2I& a, const Vector2I& b){ return a.manhattan_distance(b); }
    );
}

std::vector<Vector2I> Pathfinder::a_star_diagonal(
    const Vector2I& start,
    const Vector2I& goal
) const {
    return a_star(
        start, goal,
        [this](const Vector2I& vec){ return _grid.get_diagonal_neighbors(vec); },
        [](const Vector2I& a, const Vector2I& b) { return a.chebyshev_distance(b);}
    );
}

std::vector<Vector2I> Pathfinder::dijkstra_cardinal(
    const Vector2I& start,
    const Vector2I& goal
) const {
    return a_star(
        start, goal,
        [this](const Vector2I& vec){ return _grid.get_cardinal_neighbors(vec); },
        [](const Vector2I&, const Vector2I&) { return 0; }
    );
}

std::vector<Vector2I> Pathfinder::dijkstra_diagonal(
    const Vector2I& start,
    const Vector2I& goal
) const {
    return a_star(
        start, goal,
        [this](const Vector2I& vec){ return _grid.get_diagonal_neighbors(vec); },
        [](const Vector2I&, const Vector2I&) { return 0; }
    );
}
