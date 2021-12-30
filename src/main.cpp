#include "window.h"
#include <thread>
#include <time.h>
#include <cmath>


void sim_runner(Simulation::SimulationContext& sim) {
  while (true) {
    sim.run();
  }
}


int main() {
  Simulation::SimulationContext sim;

  constexpr size_t x_width = 1600;
  constexpr size_t y_width = 800;
  constexpr size_t z_width = 1000;
  sim.set_boundaries(x_width, y_width, z_width);

  srand(time(NULL));

  constexpr size_t number_particles = 144;
  constexpr size_t grid = std::floor(std::sqrt(number_particles));

  size_t x_spacing = (x_width - 100) / grid;
  size_t y_spacing = (y_width - 100) / grid;
  
  // 100 particles with random initial velocities

  constexpr int vmax = 500;

  for (size_t i = 0; i < number_particles; i++) {
    auto vx = rand() % vmax - (vmax / 2);
    auto vy = rand() % vmax - (vmax / 2);

    int px = (-(x_width / 2) + 100) + (i % grid) * x_spacing;
    int py = (y_width / 2) - 100 - (i / grid) * y_spacing;
   
    Simulation::Vector<float> v{static_cast<float>(vx), static_cast<float>(vy), 0.f};
    Simulation::Vector<float> p{static_cast<float>(px), static_cast<float>(py), 0.f};

    sim.add_particle(v, p);
  }

  std::thread window(Graphics::SimulationWindow, std::ref(sim));
  std::thread sim_thread(sim_runner, std::ref(sim));

  window.join();
  sim_thread.join();

  return 0;
}