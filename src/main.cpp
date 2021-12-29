#include "simulation.h"

int main() {
  Simulation::SimulationContext sim;

  Simulation::Vector<float> v1(5, 0, 0);
  Simulation::Vector<float> v2(-5, 0, 0);
  Simulation::Vector<float> p1(-200, 0, 0);
  Simulation::Vector<float> p2(200, 19, 0);

  sim.add_particle(v1, p1);
  sim.add_particle(v2, p2);
  sim.run(10000000);

  return 0;
}