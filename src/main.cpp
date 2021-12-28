#include "particle.h"
#include <iostream>
#include <array>


int main() {
  std::array<Simulation::Particle<float>, 2> particles;

  particles[0] = Simulation::Particle<float>(5, 0, 0, -200, 0, 0);
  //particles[1] = Simulation::Particle<float>(-5, 0, 0, 200, 14.142135623, 0);
  //particles[1] = Simulation::Particle<float>(-5, 0, 0, 200, 3, 0);
  particles[1] = Simulation::Particle<float>(-5, 0, 0, 200, 19, 0);

  std::cout << "system before" << std::endl;
  size_t i = 0;
  for (auto& p : particles) {
    std::cout << "Particle " << i << std::endl;
    std::cout << p << std::endl << std::endl;
    i++;
  }

  // run for 100 steps...

  bool collision = false;
  for (size_t i = 0; i < 10000000; i++) {
    // run the simulation
    for (auto& p : particles) {
      p.step();
    }
    // now check for collisions
    // we only allow 1 collision per 2 partcles per frame so the
    // one with the lower index will always "collide" first

    for (size_t j = 0; j < particles.size(); j++) {
      for (size_t k = j + 1; k < particles.size(); k++) {
        if (!collision) {
          collision = particles[j].collide(particles[k]);
        }
      }
    }
  }

  std::cout << "system after" << std::endl;
  i = 0;
  for (auto& p : particles) {
    std::cout << "Particle " << i << std::endl;
    std::cout << p << std::endl << std::endl;
    i++;
  }

  return 0;
}