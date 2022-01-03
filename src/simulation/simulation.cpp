#include "simulation.h"
#include "phys.h"

#include <algorithm>
#include <limits>

namespace Simulation {

chrono::time_point<chrono::steady_clock> SimulationContext::get_simulation_time() const {
  return m_sim_clock.now();
}

void SimulationContext::add_particle(Component::Particle<float> p) {
  add_particle_internal(p);
}

void SimulationContext::add_particle(const Component::Vector<float>& v, const Component::Vector<float> & p) {
  add_particle(Component::Particle<float>(v, p));
}

void SimulationContext::add_particle_internal(Component::Particle<float>& p) {
  size_t uid = m_working_particles.size() + 1;
  p.uid.latch(uid);
  // TODO we don't support adding particles at runtime (properly yet)
  // you can do it, it just might look weird
  m_working_particles.push_back(p);
  m_prepared_particles.push_back(p);
}

void SimulationContext::run() {
  auto now = m_sim_clock.now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(now - m_tock);

  if (elapsed > SIM_RESOLUTION_US) {
    should_calc_next_step = true;
    // copy the working buffer to the finalized buffer
    m_prepared_particles = m_working_particles;
  }

  // If we're free running, just plow ahead
  if (!m_free_run && !should_calc_next_step) {
    return;
  }
  should_calc_next_step = false;

  m_tock = chrono::time_point_cast<US_T>(now);

  // run particles
  for (auto& p : m_working_particles) {
    p.step(SIM_RESOLUTION_US);
  }
  m_step++;

  // now check for collisions
  // we only allow 1 collision per 2 partcles per frame so the
  // one with the lower index will always "collide" first
#ifdef PARALLELIZE_FOR_LOOPS
  #pragma omp parallel
#endif
  for (size_t j = 0; j < m_working_particles.size(); j++) {
    for (size_t k = j + 1; k < m_working_particles.size(); k++) {
      Status s = Physics::collide<float>(m_working_particles[j], m_working_particles[k]);
      switch(s) {
        case Status::None:
        case Status::Success:
          break;
        case Status::Impossible:
        case Status::Inconsistent:
          m_impossible_count++;
        break;
      }
    }
  }

  // check if anyone has hit a wall
  for (auto& p : m_working_particles) {
    for (const auto& w : m_boundaries) {
      if (Physics::bounce<float>(p, w) == Status::Success) {
        break;
      }
    }
  }

#ifdef DEBUG
  // print every second
  float total_energy = 0;
  static size_t last_frame = 0;
  if (m_step - last_frame > TICKS_PER_SECOND) {
    last_frame = m_step;
    std::cout << "System After Run" << std::endl;
    size_t i = 0;
    for (auto& p : m_working_particles) {
      std::cout << "Particle " << i << std::endl;
      std::cout << p << std::endl << std::endl;
      total_energy += ::powf(p.get_velocity().magnitude, 2);
      i++;
    }
    std::cout << "Total System KER: " << total_energy << std::endl;
    std::cout << "Impossible Situations: " << m_impossible_count << std::endl << std::endl;
  }
#endif
}

// stand up for yourself
void SimulationContext::set_boundaries(size_t x, size_t y, size_t z) {
  auto x_half = static_cast<float>(x) / 2.f;
  auto y_half = static_cast<float>(y) / 2.f;
  auto z_half = static_cast<float>(z) / 2.f;

  m_boundaries[WallIdx::LEFT] =   {-x_half, Component::Vector<float>(1, 0, 0),  Component::Vector<float>(-1, 1, 1)};
  m_boundaries[WallIdx::RIGHT] =  {x_half,  Component::Vector<float>(-1, 0, 0), Component::Vector<float>(-1, 1, 1)};
  m_boundaries[WallIdx::BOTTOM] = {-y_half, Component::Vector<float>(0, 1, 0),  Component::Vector<float>(1, -1, 1)};
  m_boundaries[WallIdx::TOP] =    {y_half,  Component::Vector<float>(0, -1, 0), Component::Vector<float>(0, -1, 0)};
  m_boundaries[WallIdx::BACK] =   {-z_half, Component::Vector<float>(0, 0, 1),  Component::Vector<float>(1, 1, -1)};
  m_boundaries[WallIdx::FRONT] =  {z_half,  Component::Vector<float>(0, 0, -1), Component::Vector<float>(1, 1, -1)};
}

void SimulationContext::set_free_run(bool free_run) {
  m_free_run = free_run;
}

void SimulationContext::set_settings(const SimSettings& settings) {
  m_settings = LatchingValue<SimSettings>(settings);
}

const SimSettings& SimulationContext::get_settings() const {
  return m_settings.get();
}

void SimulationContextThread(SimulationContext& sim, SimSettings settings) {
  if (settings.delay > 0) {
    auto start = chrono::steady_clock::now();
    chrono::duration<float> elapsed;
    do {
      auto now = chrono::steady_clock::now();
      elapsed = now - start;
    } while (elapsed.count() < settings.delay);
  }
  while(true) {
    sim.run();
  }
}

} // namespace Simulation