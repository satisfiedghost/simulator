#include "window.h"
#include <thread>


void sim_runner(Simulation::SimulationContext& sim) {
  while (true) {
    sim.run();
  }
}


int main() {
  Simulation::SimulationContext sim;

  Simulation::Vector<float> v1(5, 0, 0);
  Simulation::Vector<float> p1(-1000, 0, 0);

  Simulation::Vector<float> v2(0, 0, 0);
  Simulation::Vector<float> p2(0, 0, 0);

  Simulation::Vector<float> v3(0, 0, 0);
  Simulation::Vector<float> p3(-100, 0, 0);

  Simulation::Vector<float> v4(0, 0, 0);
  Simulation::Vector<float> p4(100, 0, 0);

  Simulation::Vector<float> v5(0, 0, 0);
  Simulation::Vector<float> p5(200, 0, 0);

  sim.add_particle(v1, p1);
  sim.add_particle(v2, p2);
  sim.add_particle(v3, p3);
  sim.add_particle(v4, p4);
  sim.add_particle(v5, p5);

  std::thread window(Graphics::SimulationWindow, std::ref(sim));
  std::thread sim_thread(sim_runner, std::ref(sim));

  window.join();
  sim_thread.join();

  return 0;
}