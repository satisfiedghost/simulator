#include "simulation.h"
#include<limits>
#include <algorithm>


using namespace Simulation;

chrono::time_point<chrono::steady_clock> SimulationContext::get_simulation_time() const {
  return m_sim_clock.now();
}

void SimulationContext::add_particle(Particle<float> p) {
  add_particle_internal(p);
}

void SimulationContext::add_particle(const Vector<float>& v, const Vector<float> & p) {
  add_particle(Particle<float>(v, p));
}

void SimulationContext::add_particle_internal(Particle<float>& p) {
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
  for (size_t j = 0; j < m_working_particles.size(); j++) {
    for (size_t k = j + 1; k < m_working_particles.size(); k++) {
      Status s = m_working_particles[j].collide(m_working_particles[k]);
      switch(s) {
        case Status::None:
        case Status::Success:
        break;
        case Status::Impossible:
          m_impossible_count++;
        break;
      }
    }
  }

  // check if anyone has hit a wall
  for (auto& p : m_working_particles) {
    for (const auto& w : m_boundaries) {
      // can't bounce off a wall if you're not traveling toward it....
      auto rel_vel = (p.get_velocity() * w.normal.abs());

      if (rel_vel * w.normal == rel_vel) {
        continue;
      }

      // convert to a 1D position
      auto rel_pos = (p.get_position() * w.normal.abs()).magnitude;
      if (std::abs(rel_pos - w.position) <= Particle<float>::RADIUS) {
        p.bounce(w.inverse);
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
      total_energy += std::pow(p.get_velocity().magnitude, 2);
      i++;
    }
    std::cout << "Total System KER: " << total_energy << std::endl << std::endl;
  }
#endif
}

// stand up for yourself
void SimulationContext::set_boundaries(float x, float y, float z) {
  auto x_half = x / 2;
  auto y_half = y / 2;
  auto z_half = z / 2;

  m_boundaries[WallIdx::LEFT].position = -x_half;
  m_boundaries[WallIdx::LEFT].normal = Vector<float>(1, 0, 0);
  m_boundaries[WallIdx::LEFT].inverse = Vector<float>(-1, 1, 1);

  m_boundaries[WallIdx::RIGHT].position = x_half;
  m_boundaries[WallIdx::RIGHT].normal = Vector<float>(-1, 0, 0);
  m_boundaries[WallIdx::RIGHT].inverse = Vector<float>(-1, 1, 1);

  m_boundaries[WallIdx::BOTTOM].position = -y_half;
  m_boundaries[WallIdx::BOTTOM].normal = Vector<float>(0, 1, 0);
  m_boundaries[WallIdx::BOTTOM].inverse = Vector<float>(1, -1, 1);

  m_boundaries[WallIdx::TOP].position = y_half;
  m_boundaries[WallIdx::TOP].normal = Vector<float>(0, -1, 0);
  m_boundaries[WallIdx::TOP].inverse = Vector<float>(1, -1, 1);

  m_boundaries[WallIdx::BACK].position = -z_half;
  m_boundaries[WallIdx::BACK].normal = Vector<float>(0, 0, 1);
  m_boundaries[WallIdx::BACK].inverse = Vector<float>(1, 1, -1);

  m_boundaries[WallIdx::FRONT].position = z_half;
  m_boundaries[WallIdx::FRONT].normal = Vector<float>(0, 0, -1);
  m_boundaries[WallIdx::FRONT].inverse = Vector<float>(1, 1, -1);
}

void SimulationContext::set_free_run(bool free_run) {
  m_free_run = free_run;
}

// by default, there are walls stored as far left as possible
SimulationContext::Wall::Wall()
  : position(std::numeric_limits<float>::min())
  , normal()
  , inverse()
  {}