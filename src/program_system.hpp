#pragma once

#include "program.hpp"
#include "system.hpp"
#include "registry.hpp"

class ProgramSystem : public System{
public:
    static constexpr std::size_t MAX_COMMANDS_PER_TICK = 10;

    ProgramSystem() {}

    void init(Registry& registry) override { }

    void update(Registry& registry) override {
        auto view = registry.view<Program>();

        for (entt::entity entity : view){
            Program& program = registry.get<Program>(entity);

            if (!program.started){
                program.commands[program.current_command].start(registry, entity);
                program.started = true;
            }

            std::size_t commands_started = 0;

            while (commands_started < MAX_COMMANDS_PER_TICK &&
                program.commands[program.current_command].check(registry, entity) == CommandResult::DONE
            ){
                program.commands[program.current_command].clean(registry, entity);
                ++program;
                program.commands[program.current_command].start(registry, entity);
                ++commands_started;
            }
        }
    }
};