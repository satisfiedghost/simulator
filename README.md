# Physics Simulator

A simple physics engine. Written from scratch with minimal research to see what I come up with.

# Setup

You will need the following:
* g++ or clang++.
  * g++ version tested with is 7.5.0
  * clang++ version tested with is 6.0.0
* boost (I may remove this dependency in the future, but it's needed for CLI args right now)
  * install `libboost-all-dev` with your package manager or see https://www.boost.org/users/download/
* SFML
  * install `libsfml-dev` with your package manager or see https://www.sfml-dev.org/tutorials/2.5/

To make the core program, simplay clone this repo and run `make sim`
* The resultant binary is placed in `./bin/sim`
* Run `./bin/sim` for a demo proram or `./bin/sim --help` to see available options
# Todo / Improvements / Bugs

* ~~Double buffer the simulation.~~
  * ~~Instead of idling until we're ready for the next frame, pre-calculate it before that frame and simply~~
    ~~update then. This could be doable by 2xing the particles and swapping the reference for the current one.~~
* More compiler warnings. God.
* Simulator is very upset when to spheres are ~inside each other~ occupying the same point
  * Could solve by moving them at least R * 2 apart in distance of their normal vector.
* ~~Particles stop moving at some point. Seems to be a floating point issue with the velocity.~~
* Makefile is a little flaky.
  * Should probably just move to cmake.
* More unit tests (or any really).
* Support doubles
  * Requires modifying simulation layer a bit, not too bad
* Support ints
  * Requires implementing fixed-width scalars, probably a lot of work
* 3D graphics
  * Engine already fully supports 3D particles, but we can only render 2D graphics for now
* Better name.
* ~~Performance is lacking.~~
  * ~~Better timing~~
  * ~~Maybe run simulation "on demand" and fast forward though parts where no collision is imminent.~~
  * ~~Needs real time coupling~~
* Fixing the timing issue yielded about 3 orders of magnitude increase in performance, but created some new issues.
  * Because the system is now discrete internally, particles going fast enough end up in impossible situations. Need to fix this.
* ~~Simulation boundaries~~
* Move this list to something better than a README
