#include "window.h"
#include "cli.h"
#include <thread>
#include <time.h>
#include <random>
#include <cmath>
#include <chrono>

namespace chrono = std::chrono;

// defined below, setup our initial conditions
static void set_initial_conditions(Simulation::SimulationContext& sim, Simulation::SimSettings settings);

int main(int argc, char** argv) {
  Simulation::SimulationContext sim;
  Simulation::SimSettings settings = Simulation::DefaultSettings;

  po::variables_map vm;
  if (!parse_cli_args(argc, argv, vm, settings)) {
    // bye!
    std::cout << "Failed to parse arguments, terminating." << std::endl;
    return 1;
  } else if (vm.count("help")) {
    return 0;
  }

  set_initial_conditions(sim, settings);

  std::thread window_thread(Graphics::SimulationWindowThread, std::ref(sim), settings);
  std::thread sim_thread(Simulation::SimulationContextThread, std::ref(sim), settings);

  window_thread.join();
  sim_thread.join();

  std::cout << "Goodbye!" << std::endl;
  return 0;
}

// Consider moving to its own TU at some point...
static void set_initial_conditions(Simulation::SimulationContext& sim, Simulation::SimSettings settings) {
  auto deg_to_rad = [](float angle) {
    return angle * M_PI / 180;
  };

  // TODO this should be more customizeable... but for now we just try to make this fit well on your monitor
  auto boundaries = Graphics::get_window_size();
  settings.x_width = std::get<0>(boundaries);
  settings.y_width = std::get<1>(boundaries);
  settings.z_width = std::get<2>(boundaries);

  sim.set_boundaries(settings.x_width, settings.y_width, settings.z_width);

  const size_t grid = std::ceil(std::sqrt(settings.number_particles));

  auto placement_buffer = static_cast<size_t>(std::ceil(static_cast<float>(settings.particle_radius) * 1.2));
  size_t x_spacing = (settings.x_width - placement_buffer) / grid;
  size_t y_spacing = (settings.y_width - placement_buffer) / grid;

  std::random_device rd;  // Will be used to obtain a seed for the RNG engine.
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<int> vel_dist(0, settings.vmax * 2);

  for (size_t i = 0; i < settings.number_particles; i++) {
    auto vx = 0, vy = 0;
    // vectorwise adds up to vmax
    if (!settings.display_mode) {
      if (settings.random_angle) {
        vy = vel_dist(gen) % settings.vmax - (settings.vmax / 2);
        vx = std::sqrt(std::pow(settings.vmax, 2) - std::pow(vy, 2));
        if (static_cast<int>(vx) % 2) {
          vx *= -1;
        }
      } else {
        auto rads = deg_to_rad(settings.angle);
        auto v_mag = std::max(settings.vmin, vel_dist(gen) / 2);
        vx = std::cos(rads) * v_mag;
        vy = std::sin(rads) * v_mag;
      }
    }

    Simulation::Vector<float> velocity{static_cast<float>(vx), static_cast<float>(vy), 0.f};

    if (!settings.display_mode) {
      if (velocity.magnitude < settings.vmin) {
        velocity = velocity.collinear_vector(settings.vmin);
      } else if (velocity.magnitude > settings.vmax) {
        velocity = velocity.collinear_vector(settings.vmax);
      }
    }

    int px = ((-(settings.x_width / 2)) + (i % grid) * x_spacing + placement_buffer / 2) + x_spacing / 2;
    int py = ((settings.y_width / 2) - (i / grid) * y_spacing - placement_buffer / 2) - y_spacing / 2;

    Simulation::Vector<float> position{static_cast<float>(px), static_cast<float>(py), 0.f};
    Simulation::Particle<float> particle(settings.particle_radius, velocity, position);

    sim.add_particle(particle);
  }
}