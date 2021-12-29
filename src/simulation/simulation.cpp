#include "simulation.h"

using namespace Simulation;

chrono::time_point<chrono::steady_clock> SimulationContext::get_simulation_time() const {
  return m_sim_clock.now();
}

void SimulationContext::add_particle(Particle<float> p) {
  m_particles.push_back(p);
}

void SimulationContext::add_particle(const Vector<float>& v, const Vector<float> & p) {
  m_particles.push_back(Particle<float>(v, p));
}

void SimulationContext::run() {
  auto now = m_sim_clock.now();

  if (now - m_tock < SIM_RESOLUTION_US) {
    return;
  }

  m_tock = chrono::time_point_cast<US_T>(now);

  // run particles
  for (auto& p : m_particles) {
    p.step(SIM_RESOLUTION_US);
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

#ifdef DEBUG
  // print every second
  float total_energy = 0;
  static size_t last_frame = 0;
  if (m_step - last_frame > TICKS_PER_SECOND) {
    last_frame = m_step;
    std::cout << "System After Run" << std::endl;
    size_t i = 0;
    for (auto& p : m_particles) {
      std::cout << "Particle " << i << std::endl;
      std::cout << p << std::endl << std::endl;
      total_energy += std::pow(p.get_velocity().magnitude, 2);
      i++;
    }
    std::cout << "Total System KER: " << total_energy << std::endl << std::endl;
  }
#endif
}