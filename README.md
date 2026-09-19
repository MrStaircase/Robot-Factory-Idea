# Robot Factory Idea

This is a game idea, but feel free to bring up other ideas.

## Concept: Robot Fleet Manager

You play as the operator of an automated warehouse. Your job is to design and manage a fleet of robots that move goods around the warehouse while dealing with increasingly difficult scenarios.

What it is **not**: You do **not** directly driving individual robots.

## Core Gameplay Loop
1. receive orders
2. Plan
3. Run the simulation
4. evaluate
5. expand

## Technical
Must have in the game:
- Simulation Engine: The core script that will handle the simulation
  - event system
  - Multithreaded robot/pathfinding simulation
  - Collision management
- Resource management
- Save/load system

Possibly:
- Automated tests (e.g. Unit test)
- replay system
- profiling
- CI on linux and windows

## Graphical
While this document refer to the game as robots in a warehouse, it is possible to be anything. A witch controlling many golems. A lamb that runs a cult (_Cult of the lamb_ is an actual game).
I would prefer not to call the shots in this department.

Key ideas of things to design:
- Robot/Golem/worker designs
- UI
- Animations
- Menus
- upgrading screens
