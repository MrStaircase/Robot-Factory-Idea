#include <catch2/catch_test_macros.hpp>

#include <string>
#include <unordered_map>

#include "program_system.hpp"

namespace {

using Log = std::vector<std::string>;

// All test state lives on the entity. Commands only read/write this component.
struct TestState{
    Log log;                                              // "<label>:start" / "<label>:clean"
    std::unordered_map<std::string, CommandResult> results;  // what check() reports per label
};

void log_start(const std::string& label, Registry& registry, entt::entity entity){
    registry.get<TestState>(entity).log.push_back(label + ":start");
}

void log_clean(const std::string& label, Registry& registry, entt::entity entity){
    registry.get<TestState>(entity).log.push_back(label + ":clean");
}

CommandResult report(const std::string& label, const Registry& registry, entt::entity entity){
    return registry.get<TestState>(entity).results.at(label);
}

Command make_command(const std::string& label){
    Command command;
    command.start = std::bind_front(log_start, label);
    command.check = std::bind_front(report, label);
    command.clean = std::bind_front(log_clean, label);
    return command;
}

// Creates an entity with a Program made of the given labels.
// Every command reports INPROGRESS until a test changes it.
entt::entity make_entity(Registry& registry, const std::vector<std::string>& labels){
    const entt::entity entity = registry.create_new_entity();
    registry.set<TestState>(entity, {});
    registry.set<Program>(entity, {});

    Program& program = registry.get<Program>(entity);
    TestState& state = registry.get<TestState>(entity);
    program.current_command = 0;
    for(const std::string& label : labels){
        program.commands.push_back(make_command(label));
        state.results[label] = CommandResult::INPROGRESS;
    }
    return entity;
}

TestState& state_of(Registry& registry, entt::entity entity){
    return registry.get<TestState>(entity);
}

Program& program_of(Registry& registry, entt::entity entity){
    return registry.get<Program>(entity);
}

}  // namespace

TEST_CASE("Program advances and wraps around", "[program]"){
    Program program;
    program.commands.resize(3);
    program.current_command = 0;

    SECTION("pre-increment"){
        ++program;
        CHECK(program.current_command == 1);
        ++program;
        CHECK(program.current_command == 2);
        ++program;
        CHECK(program.current_command == 0);
    }

    SECTION("post-increment"){
        program++;
        CHECK(program.current_command == 1);
        program++;
        program++;
        CHECK(program.current_command == 0);
    }
}

TEST_CASE("ProgramSystem starts only the first command on the first tick", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);
    const entt::entity entity = make_entity(registry, {"A", "B", "C"});

    system.update(registry);

    const Log expected{"A:start"};
    CHECK(state_of(registry, entity).log == expected);
    CHECK(program_of(registry, entity).started);
    CHECK(program_of(registry, entity).current_command == 0);
}

TEST_CASE("ProgramSystem leaves a running command alone", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);
    const entt::entity entity = make_entity(registry, {"A", "B"});

    system.update(registry);
    system.update(registry);
    system.update(registry);

    const Log expected{"A:start"};   // started once, never restarted or cleaned
    CHECK(state_of(registry, entity).log == expected);
    CHECK(program_of(registry, entity).current_command == 0);
}

TEST_CASE("ProgramSystem moves on when a command is done", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);
    const entt::entity entity = make_entity(registry, {"A", "B", "C"});

    system.update(registry);   // starts A
    state_of(registry, entity).results["A"] = CommandResult::DONE;
    system.update(registry);

    const Log expected{"A:start", "A:clean", "B:start"};
    CHECK(state_of(registry, entity).log == expected);
    CHECK(program_of(registry, entity).current_command == 1);
}

TEST_CASE("ProgramSystem runs instant commands within a single tick", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);
    const entt::entity entity = make_entity(registry, {"A", "B", "C"});
    state_of(registry, entity).results["A"] = CommandResult::DONE;
    state_of(registry, entity).results["B"] = CommandResult::DONE;

    system.update(registry);

    const Log expected{"A:start", "A:clean", "B:start", "B:clean", "C:start"};
    CHECK(state_of(registry, entity).log == expected);
    CHECK(program_of(registry, entity).current_command == 2);
}

TEST_CASE("ProgramSystem wraps around to the first command", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);
    const entt::entity entity = make_entity(registry, {"A", "B"});

    state_of(registry, entity).results["A"] = CommandResult::DONE;
    system.update(registry);   // A:start, A:clean, B:start, B in progress
    REQUIRE(program_of(registry, entity).current_command == 1);

    state_of(registry, entity).results["A"] = CommandResult::INPROGRESS;
    state_of(registry, entity).results["B"] = CommandResult::DONE;
    system.update(registry);

    const Log expected{"A:start", "A:clean", "B:start", "B:clean", "A:start"};
    CHECK(state_of(registry, entity).log == expected);
    CHECK(program_of(registry, entity).current_command == 0);
}

TEST_CASE("ProgramSystem does not advance past a failed command", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);
    const entt::entity entity = make_entity(registry, {"A", "B"});
    state_of(registry, entity).results["A"] = CommandResult::FAILURE;

    system.update(registry);
    system.update(registry);

    // Documents current behavior: a failure stalls the program on that command.
    // Whether clean should run on failure is still an open design question.
    const Log expected{"A:start"};
    CHECK(state_of(registry, entity).log == expected);
    CHECK(program_of(registry, entity).current_command == 0);
}

TEST_CASE("ProgramSystem caps the commands handled per tick", "[program][system]"){
    const std::size_t limit = ProgramSystem::MAX_COMMANDS_PER_TICK;

    Registry registry;
    ProgramSystem system;
    system.init(registry);

    SECTION("a program of only instant commands stops at the cap and resumes next tick"){
        const entt::entity entity = make_entity(registry, {"A", "B", "C"});
        state_of(registry, entity).results["A"] = CommandResult::DONE;
        state_of(registry, entity).results["B"] = CommandResult::DONE;
        state_of(registry, entity).results["C"] = CommandResult::DONE;

        system.update(registry);

        // 1 initial start, then `limit` transitions of (clean + start)
        CHECK(state_of(registry, entity).log.size() == 1 + 2 * limit);
        CHECK(program_of(registry, entity).current_command == limit % 3);

        system.update(registry);

        CHECK(state_of(registry, entity).log.size() == 1 + 4 * limit);
        CHECK(program_of(registry, entity).current_command == (2 * limit) % 3);
    }

    SECTION("a single instant command loops on itself without hanging"){
        const entt::entity entity = make_entity(registry, {"A"});
        state_of(registry, entity).results["A"] = CommandResult::DONE;

        system.update(registry);

        CHECK(state_of(registry, entity).log.size() == 1 + 2 * limit);
        CHECK(program_of(registry, entity).current_command == 0);
    }
}

TEST_CASE("ProgramSystem keeps entities independent", "[program][system]"){
    Registry registry;
    ProgramSystem system;
    system.init(registry);

    const entt::entity first = make_entity(registry, {"A", "B"});
    const entt::entity second = make_entity(registry, {"A", "B"});
    state_of(registry, first).results["A"] = CommandResult::DONE;

    system.update(registry);

    const Log expected_first{"A:start", "A:clean", "B:start"};
    const Log expected_second{"A:start"};
    CHECK(state_of(registry, first).log == expected_first);
    CHECK(state_of(registry, second).log == expected_second);
    CHECK(program_of(registry, first).current_command == 1);
    CHECK(program_of(registry, second).current_command == 0);
}