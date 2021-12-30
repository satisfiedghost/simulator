#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <array>
#include <numeric>
#include <algorithm>
#include "simulation.h"

class SimulationTest :
  public ::testing::Test {};

// A baseline performance test to alert us if we've regressed there.
// Essentially taking the demo program and baselining how long it takes on my netbook...

constexpr size_t x_width = 1600;
constexpr size_t y_width = 800;
constexpr size_t z_width = 1000;
constexpr size_t N_STEPS = 10000UL;
// This was consistently coming out to roughly 4.9s, so let's be strict for now and trigger if it goes above 5

// this seems to be worst case about 12 seconds with gcc, and 8 seconds with clang

#if defined(__clang__)
constexpr size_t TIME_LIMIT_US = 8e6;
#elif defined(__GNUC__)
constexpr size_t TIME_LIMIT_US = 12e6;
#endif

// Yes this should all become part of the above class but I'm feeling lazy right now and just want this number
namespace chrono = chrono;


void sim_runner(Simulation::SimulationContext& sim, std::array<int64_t, N_STEPS>& cycle_times_us) {
  sim.set_free_run(true);
  for (size_t i = 0; i < N_STEPS; i++) {
    auto start = chrono::steady_clock::now();
    sim.run();
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cycle_times_us[i] = elapsed;
  }
}

double calculate_average(std::array<int64_t, N_STEPS>& cycle_times) {
  auto sum = std::accumulate(cycle_times.begin(), cycle_times.end(), 0);
  return static_cast<double>(sum) / static_cast<double>(cycle_times.size());
}

// copy the array becuase nth_element sorts the other elements non-deterministically...
int64_t calculate_median(std::array<int64_t, N_STEPS> cycle_times_us) {
  size_t n = cycle_times_us.size() / 2;
  std::nth_element(cycle_times_us.begin(), cycle_times_us.begin() + n, cycle_times_us.end());
  return cycle_times_us[n];
}


TEST_F(SimulationTest, Performance) {
  Simulation::SimulationContext sim;

  sim.set_boundaries(x_width, y_width, z_width);

  srand(time(NULL));

  constexpr size_t number_particles = 400;
  const size_t grid = std::floor(std::sqrt(number_particles));

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

  std::array<int64_t, N_STEPS> cycle_times_us;

  auto start = chrono::steady_clock::now();
  std::thread sim_thread(sim_runner, std::ref(sim), std::ref(cycle_times_us));
  sim_thread.join();
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();

  chrono::duration<float> seconds = end - start;

  std::cout << "Ran " << N_STEPS << " Cycles" << std::endl;
  std::cout << "Elapsed Time: " << seconds.count() << "s" << std::endl;
  std::cout << "Average Run Cycle Time: " << calculate_average(cycle_times_us) << "us" << std::endl;
  std::cout << "Median Run Cycle Time: " << calculate_median(cycle_times_us) << "us" << std::endl;
  std::cout << "Max Run Cycle Time: " << *std::max_element(cycle_times_us.begin(), cycle_times_us.end()) << "us" << std::endl;
  std::cout << "Min Run Cycle Time: " << *std::min_element(cycle_times_us.begin(), cycle_times_us.end()) << "us" << std::endl;

  ASSERT_LT(elapsed, TIME_LIMIT_US);
}
