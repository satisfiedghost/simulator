#include "simulation.h"

using namespace Simulation;

std::chrono::microseconds SimulationContext::get_simulation_time() const {
  return m_sim_time;
}

void SimulationContext::add_particle(Particle<float> p) {
  m_particles.push_back(p);
}

void SimulationContext::add_particle(const Vector<float>& v, const Vector<float> & p) {
  m_particles.push_back(Particle<float>(v, p));
}

void SimulationContext::run(size_t steps) {

  for (size_t i = 0; i < steps; i++) {
    // run particles
    for (auto& p : m_particles) {
      p.step();
    }
    m_step++;

    // now check for collisions
    // we only allow 1 collision per 2 partcles per frame so the
    // one with the lower index will always "collide" first
    for (size_t j = 0; j < m_particles.size(); j++) {
      for (size_t k = j + 1; k < m_particles.size(); k++) {
        m_particles[j].collide(m_particles[k]);
      }
    }
  }

#ifdef DEBUG
  constexpr size_t step_rate_limit = 1e6;
  static size_t last_frame = 0;
  if (m_step - last_frame > step_rate_limit) {
    last_frame = m_step;
    std::cout << "System After Run" << std::endl;
    size_t i = 0;
    for (auto& p : m_particles) {
      std::cout << "Particle " << i << std::endl;
      std::cout << p << std::endl << std::endl;
      i++;
    }
  }
#endif
}