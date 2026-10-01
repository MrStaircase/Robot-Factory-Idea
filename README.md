# Robot Factory Idea

This is a game idea, but feel free to bring up other ideas.

## Concept: Robot Fleet Manager

### Inspirations:

- [SpaceChem](https://store.steampowered.com/app/92800/SpaceChem/): **puzzle game**, create a looping factory of simultanious workers, contains a **leaderboard** of quickest solution, cheapest solution etc.
- [Autonauts](https://store.steampowered.com/app/979120/Autonauts/): build a prospourous **colony** by "programming" robots, that will run their instructions to do anything you can do. Unlocks gradually over time.
- [The Farmer Was Replaced](https://store.steampowered.com/app/2060160/The_Farmer_Was_Replaced/): **incemental/idle game**, the player programs in a python extension a flying drone to accumulate more and more resources. also includes some puzzles.

In my opinion, The Farmer Was Replaced, put too much focus into coding and not enough on the gamification of the code. While the level design of SpaceChem might be the "easiest" thing to design, I believe that the approach autonauts take is the best implementation of all.

### The Idea

The goal of the game will be to reach some sort of automation goal (e.g. automate the creation of a object / reach some productivity goal of 100 items per minute).

This automation is done by a number of programmable entities (hereinafter, robots) which will carry the tasks assign to them. At first the player will only have few such robots that will grant him resources (e.g. money, wood, according to the theme). With this resources the player may purchase new instructions, robots, etc.

It is important to note that the player is not in fact creating or doing the tasks himself. He is merely a manager of those robots (maybe he is far away, maybe he is just too lazy to do anything).

According to our decision (which I believe will only be solidified much later in developmet), the player may have access to few more sophisticated robots or many simpler robots that are distinct. Similarly, the acqusition of new robots may be a fixed price or a gradual one (e.g. at first 1 gold, but later 1000 gold).

In terms of graphical design, though they are called robots, they can be anything we desire: Drones, Golems with inscriptions, Animals, Differet shapes of Origami, and much more. Since the core of this game surrounds this entities, it might be a good idea to come up with a few graphical designs (or inspirations) from which we can decide what fits the best.

I have a preference to a 2D world, which I believe fits both our scope and skills. As for graphical style, I have no input to give.

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
