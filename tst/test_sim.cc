#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <iostream>
#include "simulation.h"

class SimulationTest :
  public ::testing::Test {};

// A baseline performance test to alert us if we've regressed there.
// Essentially taking the demo program and baselining how long it takes on my netbook...

constexpr size_t x_width = 1600;
constexpr size_t y_width = 800;
constexpr size_t z_width = 1000;
constexpr size_t steps = 10000UL;
// Yes this should become part of the above class but I'm feeling lazy right now and just want this number

void sim_runner(Simulation::SimulationContext& sim) {
  sim.set_free_run(true);
  for (size_t i = 0; i < steps; i++) {
    sim.run();
  }
}


TEST_F(SimulationTest, Performance) {
  Simulation::SimulationContext sim;

  sim.set_boundaries(x_width, y_width, z_width);

  srand(time(NULL));

  constexpr size_t number_particles = 400;
  constexpr size_t grid = std::floor(std::sqrt(number_particles));

  size_t x_spacing = (x_width - 100) / grid;
  size_t y_spacing = (y_width - 100) / grid;
  
  // 100 particles with random initial velocities

  constexpr int vmax = 100;

  for (size_t i = 0; i < number_particles; i++) {
    auto vx = rand() % vmax - (vmax / 2);
    auto vy = rand() % vmax - (vmax / 2);

    int px = (-(x_width / 2) + 100) + (i % grid) * x_spacing;
    int py = (y_width / 2) - 100 - (i / grid) * y_spacing;
   
    Simulation::Vector<float> v{static_cast<float>(vx), static_cast<float>(vy), 0.f};
    Simulation::Vector<float> p{static_cast<float>(px), static_cast<float>(py), 0.f};

    sim.add_particle(v, p);
  }

  auto start = std::chrono::steady_clock::now();
  std::thread sim_thread(sim_runner, std::ref(sim));
  sim_thread.join();
  auto end = std::chrono::steady_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

  std::cout << "Elapsed Time: " << elapsed << "us" << std::endl;

  // This was consistently coming out to roughly 4.9s, so let's be strict for now and trigger if it goes above 5
  ASSERT_LT(elapsed, 5e6);
}
