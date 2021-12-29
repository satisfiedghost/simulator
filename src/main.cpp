#include "simulation.h"

int main() {
  Simulation::SimulationContext sim;

  Simulation::Vector<float> v1(5, 0, 0);
  Simulation::Vector<float> v2(-5, 0, 0);
  Simulation::Vector<float> p1(-200, 0, 0);
  Simulation::Vector<float> p2(200, 11, 0);
  Simulation::Vector<float> v3(0, 0, 0);
  Simulation::Vector<float> p3(0, 25, 0);

  sim.add_particle(v1, p1);
  sim.add_particle(v2, p2);
  sim.add_particle(v3, p3);
  sim.run(10000000);

  return 0;
}