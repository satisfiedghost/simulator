#include "demo/demo.h"
#include "window.h"

#include <chrono>
#include <cmath>
#include <thread>
#include <time.h>
#include <vector>

namespace Demo {

// for generating initial conditions of the simulation, we don't really mind this
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"

namespace chrono = std::chrono;

template<typename V>
static void correct_overlap(std::vector<Component::Particle<V>>& particles, std::mt19937& gen);

template <typename V>
static bool validate_free(const std::vector<Component::Particle<V>>& particles,
                          typename V::vector_t radius, V pos);

template<typename V>
void set_initial_conditions(Simulation::SimulationContext<V>& sim, Simulation::SimSettings<typename V::vector_t> settings) {
  typedef typename V::vector_t vector_t;

  auto deg_to_rad = [](float angle) {
    return angle * M_PI / 180;
  };

  // TODO this should be more customizeable... but for now we just try to make this fit well on your monitor
  auto boundaries = Graphics::get_window_size<vector_t>();
  settings.x_width = std::get<0>(boundaries);
  settings.y_width = std::get<1>(boundaries);
  settings.z_width = std::get<2>(boundaries);

  sim.set_boundaries(settings.x_width, settings.y_width, settings.z_width);

  const size_t grid = static_cast<size_t>(std::ceil(std::sqrt(settings.number_particles)));

  auto placement_buffer = static_cast<size_t>(std::ceil(static_cast<float>(settings.radius_max) * 1.2));
  size_t x_spacing = (settings.x_width - placement_buffer) / grid;
  size_t y_spacing = (settings.y_width - placement_buffer) / grid;

  std::random_device rd;  // Will be used to obtain a seed for the RNG engine.
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<float> vel_dist(0, static_cast<float>(settings.vmax));
  std::uniform_real_distribution<float> mass_dist(static_cast<float>(settings.mass_min), static_cast<float>(settings.mass_min));
  std::uniform_int_distribution<int> radius_dist(static_cast<float>(settings.radius_min), static_cast<float>(settings.radius_max));
  std::uniform_real_distribution<float> rad_dist(0, 2 * M_PI);

  std::vector<Component::Particle<V>> particles;

  for (size_t i = 0; i < settings.number_particles; i++) {
    auto v_mag = vel_dist(gen);
    v_mag = (v_mag < static_cast<float>(settings.vmin)) ? static_cast<float>(settings.vmin) : v_mag;
    auto vx = 0.f, vy = 0.f;
    // vectorwise adds up to vmax
    if (!settings.display_mode) {
      if (settings.random_angle) {
        auto random_rad = rad_dist(gen);
        vx = std::cos(random_rad) * v_mag;
        vy = std::sin(random_rad) * v_mag;
      } else {
        auto rads = deg_to_rad(settings.angle);
        vx = std::cos(rads) * v_mag;
        vy = std::sin(rads) * v_mag;
      }
    }

    Component::Vector<vector_t> velocity{vector_t(vx), vector_t(vy), vector_t(0.f)};

    int px = ((-(settings.x_width / 2)) + (i % grid) * x_spacing + placement_buffer / 2) + x_spacing / 2;
    int py = ((settings.y_width / 2) - (i / grid) * y_spacing - placement_buffer / 2) - y_spacing / 2;

    auto mass = mass_dist(gen);
    auto radius = radius_dist(gen);

    Component::Vector<vector_t> position{vector_t(px), vector_t(py), vector_t(0.f)};
    Component::Particle<V> particle(radius, mass, velocity, position);

    particles.push_back(particle);
  }
  // check for overlap, common with disparate radii. TODO make this not suck
  correct_overlap(particles, gen);

  // add particles in known good initial state
  for (const auto& p : particles) {
    sim.add_particle(p);
  }
}

template <typename V>
static bool validate_free(const std::vector<Component::Particle<V>>& particles,
                          typename V::vector_t radius, V pos) {
  for (const auto& p : particles) {
    if ((p.position() - pos).magnitude() < radius * Simulation::DefaultSettings<typename V::vector_t>.overlap_detection) {
      return false;
    }
  }
  return true;
}

template<typename V>
void correct_overlap(std::vector<Component::Particle<V>>& particles, std::mt19937& gen) {
  std::uniform_real_distribution<float> offset_dir(0, 2 * M_PI);

  bool user_warned = false;
  bool overlap_detected = false;
  do {
    overlap_detected = false;
    for (size_t i = 0; i < particles.size(); i++) {
      for (size_t j = i + 1; j < particles.size(); j++) {
        auto& a = particles[i];
        auto& b = particles[j];
        auto dist = a.position() - b.position();
        auto min_dist = a.radius() + b.radius();

        // move over!
        if (dist.magnitude() < min_dist) {
          overlap_detected = true;
          if (!user_warned) {
            std::cout << "Modifying your particle positioning to fit particles on this screen..." << std::endl;
            user_warned = true;
          }
          // smaller one gets moved, less likely to cause compounding issues
          auto& mover = (a.radius() < b.radius()) ? a : b;
          const auto& stayer = (mover == a) ? b : a;

          // this is truly horrible but we only do it once at init...
          bool found_free_space = false;
          do {
            // randomly project out from B
            auto dir = offset_dir(gen);

            V offset{std::cos(dir),
                     std::sin(dir),
                     0};

            // proposed new position
            auto new_pos = stayer.position() + 1.1 * offset.collinear_vector(a.radius() + b.radius());
            if (validate_free(particles, mover.radius(), new_pos)) {
              found_free_space = true;
              mover = Component::Particle<V>(mover.radius(), mover.mass(), mover.velocity(), new_pos);
            }
          } while(!found_free_space);
        }
      }
    }
  }while(overlap_detected);
}


template
void set_initial_conditions(Simulation::SimulationContext<Component::Vector<float>>&,
  Simulation::SimSettings<typename Component::Vector<float>::vector_t>);

template
void set_initial_conditions(Simulation::SimulationContext<Component::Vector<double>>&,
  Simulation::SimSettings<typename Component::Vector<double>::vector_t>);

template
void set_initial_conditions(Simulation::SimulationContext<Component::Vector<Util::FixedPoint>>&,
  Simulation::SimSettings<typename Component::Vector<Util::FixedPoint>::vector_t>);

} // Demo