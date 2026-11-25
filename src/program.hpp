#pragma once

#include "registry.hpp"
#include <vector>
#include <functional>

enum class CommandResult{
    DONE,
    INPROGRESS,
    FAILURE
};

struct Command{
    std::function<void(Registry&, entt::entity)> start;
    std::function<CommandResult(const Registry&, entt::entity)> check;
    std::function<void(Registry&, entt::entity)> clean;
};

class Program{
public:
    std::vector<Command> commands;
    std::size_t current_command = 0;
    bool started = false;

    Program& operator++(){
        ++current_command;
        
        if (current_command == commands.size()){
            current_command = 0;
        }

        return *this;
    }

    Program& operator++(int){
        current_command++;
        
        if (current_command == commands.size()){
            current_command = 0;
        }

        return *this;
    }
};
