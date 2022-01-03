# Physics Simulator

A physics engine. Written from scratch with minimal research to see what I come up with.

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
