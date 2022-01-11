#include "context.h"
#include "debug.h"
#include "info.h"

#include <algorithm>
#include <limits>
#include <thread>

namespace Simulation {

template<typename T>
chrono::time_point<chrono::steady_clock> SimulationContext<T>::get_simulation_time() const {
  return m_sim_clock.now();
}

template<typename T>
chrono::microseconds SimulationContext<T>::get_elapsed_time_us() const {
  auto now = get_simulation_time();
  return chrono::duration_cast<chrono::microseconds>(now - m_start);
}

template<typename T>
void SimulationContext<T>::add_particle(Component::Particle<T> p) {
  add_particle_internal(p);
}

template<typename T>
void SimulationContext<T>::add_particle(const Component::Vector<T>& v, const Component::Vector<T> & p) {
  add_particle(Component::Particle<T>(v, p));
}

template<typename T>
void SimulationContext<T>::add_particle_internal(Component::Particle<T>& p) {
  p.uid.latch(m_particle_count + 1);
  // TODO we don't support adding particles at runtime (properly yet)
  // you can do it, it just might look weird
  m_particle_count++;
  m_particle_buffer.push_back(p);
}

template<typename T>
void SimulationContext<T>::run() {
  auto now = m_sim_clock.now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(now - m_tock);

  if (elapsed > SIM_RESOLUTION_US) {
    should_calc_next_step = true;
  }

  // If we're free running, just plow ahead
  if (!m_free_run && !should_calc_next_step) {
    return;
  }

  // whistle and wait
  std::shared_ptr<std::vector<Component::Particle<T>>> particles;
  while (m_particle_buffer.get_writeable(particles) == Status::NotReady) {}

  should_calc_next_step = false;
  m_tock = chrono::time_point_cast<US_T>(now);

  // Gravity rides everything
  for (auto& p : *particles) {
    m_physics_context.gravity(p, SIM_RESOLUTION_US);
  }

  // run particles
  for (auto& p : *particles) {
    m_physics_context.step(p, SIM_RESOLUTION_US);
  }

  // now check for collisions
  // we only allow 1 collision per 2 partcles per frame so the
  // one with the lower index will always "collide" first
#ifdef PARALLELIZE_FOR_LOOPS
  #pragma omp parallel
#endif
  for (size_t j = 0; j < particles->size(); j++) {
    for (size_t k = j + 1; k < particles->size(); k++) {
      Status s = m_physics_context.collide((*particles)[j], (*particles)[k]);
      switch(s) {
        case Status::None:
        break;
        case Status::Inconsistent:
          m_inconsistent_count++;
          m_collision_count++;
        break;
        case Status::Corrected:
          m_correction_count++;
          m_collision_count++;
        break;
        case Status::Success:
          m_collision_count++;
        break;
        default:
        break;
      }
    }
  }

  // check if anyone has hit a wall
  for (auto& p : *particles) {
    for (const auto& w : m_boundaries) {
      // we allow multiple bounces per frame in case e.g. a particle goes into a corner
      if (m_physics_context.bounce(p, w) == Status::Success) {
        m_bounce_count++;
      }
    }
  }

  INFO_MSG(SYSTEM_STATUS);
  DEBUG_MSG(SYSTEM_REPORT);

  m_particle_buffer.put();
  m_step++;
}

// stand up for yourself
template<typename T>
void SimulationContext<T>::set_boundaries(size_t x, size_t y, size_t z) {
  auto x_half = static_cast<T>(x) / static_cast<T>(2);
  auto y_half = static_cast<T>(y) / static_cast<T>(2);
  auto z_half = static_cast<T>(z) / static_cast<T>(2);

  m_boundaries[Component::WallIdx::LEFT]   = std::move(Component::Wall<T>({-x_half,
                                                       Component::Vector<T>(1, 0, 0),
                                                       Component::Vector<T>(-1, 1, 1),
                                                       Component::WallIdx::LEFT}));

  m_boundaries[Component::WallIdx::RIGHT]  = std::move(Component::Wall<T>({x_half,
                                                       Component::Vector<T>(-1, 0, 0),
                                                       Component::Vector<T>(-1, 1, 1),
                                                       Component::WallIdx::RIGHT}));

  m_boundaries[Component::WallIdx::BOTTOM] = std::move(Component::Wall<T>({-y_half,
                                                       Component::Vector<T>(0, 1, 0),
                                                       Component::Vector<T>(1, -1, 1),
                                                       Component::WallIdx::BOTTOM}));

  m_boundaries[Component::WallIdx::TOP]    = std::move(Component::Wall<T>({y_half,
                                                       Component::Vector<T>(0, -1, 0),
                                                       Component::Vector<T>(1, -1, 1),
                                                       Component::WallIdx::TOP}));

  m_boundaries[Component::WallIdx::BACK]   = std::move(Component::Wall<T>({-z_half,
                                                       Component::Vector<T>(0, 0, 1),
                                                       Component::Vector<T>(1, 1, -1),
                                                       Component::WallIdx::BACK}));

  m_boundaries[Component::WallIdx::FRONT]  = std::move(Component::Wall<T>({z_half,
                                                       Component::Vector<T>(0, 0, -1),
                                                       Component::Vector<T>(1, 1, -1),
                                                       Component::WallIdx::FRONT}));

}

template<typename T>
void SimulationContext<T>::set_free_run(bool free_run) {
  m_free_run = free_run;
}

template<typename T>
void SimulationContext<T>::set_settings(const SimSettings& settings) {
  m_settings = Util::LatchingValue<SimSettings>(settings);
}

template<typename T>
const SimSettings& SimulationContext<T>::get_settings() const {
  return m_settings.get();
}

template<typename T>
void SimulationContextThread(SimulationContext<T>& sim, SimSettings settings) {
  sim.set_settings(settings);

  if (settings.display_mode) {
    while(true) {}
  }

  if (settings.delay > 0) {
    auto start = chrono::steady_clock::now();
    chrono::duration<float> elapsed;
    do {
      auto now = chrono::steady_clock::now();
      elapsed = now - start;
    } while (elapsed.count() < settings.delay);
  }

  // startup the buffer-copy thread
  std::thread ring_buffer_copy_thread =
        std::thread(Util::ring_thread<std::vector<Component::Particle<T>>,
                                      Component::Particle<T>,
                                      SimSettings::RingBufferSize>,
                                      std::ref(sim.m_particle_buffer));
  ring_buffer_copy_thread.detach();

  // make 2 copies of the simulation at the start.. allows for corrections if an error occurs on the first frame
  sim.m_particle_buffer.put();

  while(true) {
    sim.run();
  }
}

template class SimulationContext<float>;
template class SimulationContext<double>;

template void SimulationContextThread(SimulationContext<float>& sim, SimSettings settings);
template void SimulationContextThread(SimulationContext<double>& sim, SimSettings settings);

} // namespace Simulation
