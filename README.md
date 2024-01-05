# Physics Simulator

A physics engine. Written from scratch with minimal research to see what I come up with.

Essentially, the engine simulates perfectly elastic collisions between spheres. Despite the below gif, the engine is actually fully 3D capable! I am yet to get to rendering 3D graphics, so for now all objects have a zero Z-component to their positions and velocities.

At this point it feels more like an interesting visualizer than anything with which you might interact. It supports setting up a lot of initial conditions (color, kinetic energy, direction, etc.), as well as randomizing these attributes.

![demo](./demo.gif)

# What gives, sometimes the spheres clip or interact strangely!

When writing this origiinally, I noticed a lot of strange behavior. Simulating the real world with any fidelity in a discrete system with limited computational power turned out to be difficult. Shocker, I know. The original engine was internally based on all floating point math. When run for long enough (except in a few well-ordered initial setups that would repeat) a collision would eventually occur which left net kinetic energy imparted on the spheres involved. The system would accumulate errors and quickly become entropic, and our beautiful shapes would gain enough energy to clip through their container and fly off to oblivion.

I thought I could fix this by switching to fixed-point arithmetic. The problem persisted. As a workaround, I now check the total energy before a collision, and the resultant summed energy after. If it's greater than some delta, we attempt to correct by re-runnign the simulation with a higher fidelity. We do this several times and if the result is still too disparate, we ignore the collision and retian the original state of the spheres. They will attempt to collide again upon the next frame and so on. This can lead to glitches in the system. It irks me quite a bit and is the next thing I'll likely fix with it.

# Setup

You will need the following:
* g++ or clang++.
  * g++ version tested with is 7.5.0
  * clang++ version tested with is 6.0.0
* boost (I may remove this dependency in the future, but it's needed for CLI args right now)
  * install `libboost-all-dev` with your package manager or see https://www.boost.org/users/download/
* SFML
  * install `libsfml-dev` with your package manager or see https://www.sfml-dev.org/tutorials/2.5/

To make the core program, simply clone this repo and run `make sim`
* The resultant binary is placed in `./bin/sim`
* Run `./bin/sim` for a demo proram or `./bin/sim --help` to see available options

# Acknowledgements

* A big thanks to my physics friends for letting me endlessly barrage them with questions about newtonian physics.
* This stackoverflow post is likely what made this possible https://physics.stackexchange.com/questions/107648/what-are-the-general-solutions-to-a-hard-sphere-collision
