#define private public  // they're just words do whatever you want
#include "simulation.h"
#define private private

#include <algorithm>
#include <array>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <numeric>
#include <thread>

class SimulationTest :
  public ::testing::Test {};

// A baseline performance test to alert us to regressions.
// this seems to be worst case about 12 seconds with gcc, and 8 seconds with clang
// Yes this should all become part of the above class but I'm feeling lazy right now and just want this number
#if defined(__clang__)
constexpr size_t TIME_LIMIT_US = 6e6;
#elif defined(__GNUC__)
constexpr size_t TIME_LIMIT_US = 8e6;
#endif

namespace chrono = chrono;
using Component::Vector;
using Component::Particle;

struct TestSettings {
  size_t n_particles = 400;
  size_t x_width = 1000;
  size_t y_width = 1000;
  size_t z_width = 1000;
  float mass_min = 1;
  float mass_max = 10;
  size_t radius_min = 10;
  size_t radius_max = 20;
  float v_max = 100;
  static constexpr size_t N_STEPS = 10000UL;
}TestSettings;

template<typename T>
void sim_runner(Simulation::SimulationContext<T>& sim, std::array<int64_t, TestSettings::N_STEPS>& cycle_times_us) {

  // simulation blocks forever if this isn't running...

  std::thread th = std::thread(Util::ring_thread<std::vector<Component::Particle<T>>,
                                                 Component::Particle<T>,
                                                 Simulation::SimSettings::RingBufferSize>,
                                                 std::ref(sim.m_particle_buffer));
  th.detach();

  sim.set_free_run(true);
  for (size_t i = 0; i < TestSettings::N_STEPS; i++) {
    auto start = chrono::steady_clock::now();
    sim.run();
    auto end = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cycle_times_us[i] = elapsed;
  }
}

double calculate_average(std::array<int64_t, TestSettings::N_STEPS>& cycle_times) {
  auto sum = std::accumulate(cycle_times.begin(), cycle_times.end(), 0);
  return static_cast<double>(sum) / static_cast<double>(cycle_times.size());
}

// copy the array becuase nth_element sorts the other elements non-deterministically...
int64_t calculate_median(std::array<int64_t, TestSettings::N_STEPS> cycle_times_us) {
  size_t n = cycle_times_us.size() / 2;
  std::nth_element(cycle_times_us.begin(), cycle_times_us.begin() + n, cycle_times_us.end());
  return cycle_times_us[n];
}

TEST_F(SimulationTest, Performance) {
  Simulation::SimulationContext<float> sim;

  sim.set_boundaries(TestSettings.x_width, TestSettings.y_width, TestSettings.z_width);

  // we want a deterministic benchmark
  srand(0xDEADBEEF);

  const size_t number_particles = TestSettings.n_particles;
  const size_t grid = std::floor(std::sqrt(number_particles));

  size_t x_spacing = (TestSettings.x_width - 100) / grid;
  size_t y_spacing = (TestSettings.y_width - 100) / grid;

  // 100 particles with random initial velocities

  const int vmax = TestSettings.v_max;

  std::cout << "Creating and adding particles...." << std::endl;
  for (size_t i = 0; i < number_particles; i++) {
    auto vx = rand() % vmax - (vmax / 2);
    auto vy = rand() % vmax - (vmax / 2);
    auto radius = std::max(TestSettings.radius_min, rand() % (TestSettings.radius_max + 1));

    auto mass_range_map = rand() % 101;
    auto mass = TestSettings.mass_min +
                (static_cast<float>(mass_range_map) / 100.f) * (TestSettings.mass_max - TestSettings.mass_min);

    int px = (-(TestSettings.x_width / 2) + 100) + (i % grid) * x_spacing;
    int py = (TestSettings.y_width / 2) - 100 - (i / grid) * y_spacing;

    Vector<float> v{static_cast<float>(vx), static_cast<float>(vy), 0.f};
    Vector<float> p{static_cast<float>(px), static_cast<float>(py), 0.f};


    Particle<float> particle(radius, mass, v, p);
    particle.uid.latch(i + 1);

    sim.add_particle(particle);
  }
  std::cout << std::endl << "Done! Running performance benchmark..." << std::endl;

  std::array<int64_t, TestSettings::N_STEPS> cycle_times_us;

  auto start = chrono::steady_clock::now();
  std::thread sim_thread(sim_runner<float>, std::ref(sim), std::ref(cycle_times_us));
  sim_thread.join();
  auto end = chrono::steady_clock::now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();

  chrono::duration<float> seconds = end - start;

  std::cout << "Ran " << TestSettings::N_STEPS << " Cycles" << std::endl;
  std::cout << "Elapsed Time: " << seconds.count() << "s" << std::endl;
  std::cout << "Average Run Cycle Time: " << calculate_average(cycle_times_us) << "us" << std::endl;
  std::cout << "Median Run Cycle Time: " << calculate_median(cycle_times_us) << "us" << std::endl;
  std::cout << "Max Run Cycle Time: " << *std::max_element(cycle_times_us.begin(), cycle_times_us.end()) << "us" << std::endl;
  std::cout << "Min Run Cycle Time: " << *std::min_element(cycle_times_us.begin(), cycle_times_us.end()) << "us" << std::endl;

  ASSERT_LT(elapsed, TIME_LIMIT_US);
}
