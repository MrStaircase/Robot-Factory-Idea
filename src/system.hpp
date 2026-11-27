#pragma once

#include "registry.hpp"

class System{
public:
    ~System() = default;

    virtual void init(Registry& registry) = 0;
    virtual void update(Registry& registry) = 0;
};