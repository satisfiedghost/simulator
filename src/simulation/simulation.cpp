#include "context.h"
#include "debug.h"
#include "info.h"

#include <algorithm>
#include <limits>
#include <thread>

namespace Simulation {

template<typename V>
chrono::time_point<chrono::steady_clock> SimulationContext<V>::get_simulation_time() const {
  return m_sim_clock.now();
}

template<typename V>
chrono::microseconds SimulationContext<V>::get_elapsed_time_us() const {
  auto now = get_simulation_time();
  return chrono::duration_cast<chrono::microseconds>(now - m_start);
}

template<typename V>
void SimulationContext<V>::add_particle(Component::Particle<V> p) {
  add_particle_internal(p);
}

template<typename V>
void SimulationContext<V>::add_particle(const V& v, const V& p) {
  add_particle(Component::Particle<V>(v, p));
}

template<typename V>
void SimulationContext<V>::add_particle_internal(Component::Particle<V>& p) {
  p.uid.latch(m_particle_count + 1);
  // TODO we don't support adding particles at runtime (properly yet)
  // you can do it, it just might look weird
  m_particle_count++;
  m_particle_buffer.push_back(p);
}

template<typename V>
void SimulationContext<V>::run() {
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
  std::shared_ptr<std::vector<Component::Particle<V>>> particles;
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
template<typename V>
void SimulationContext<V>::set_boundaries(size_t x, size_t y, size_t z) {
  auto x_half = static_cast<typename V::vector_t>(x) / static_cast<typename V::vector_t>(2);
  auto y_half = static_cast<typename V::vector_t>(y) / static_cast<typename V::vector_t>(2);
  auto z_half = static_cast<typename V::vector_t>(z) / static_cast<typename V::vector_t>(2);

  m_boundaries[Component::WallIdx::LEFT]   = std::move(Component::Wall<V>({-x_half,
                                                       V(1, 0, 0),
                                                       V(-1, 1, 1),
                                                       Component::WallIdx::LEFT}));

  m_boundaries[Component::WallIdx::RIGHT]  = std::move(Component::Wall<V>({x_half,
                                                       V(-1, 0, 0),
                                                       V(-1, 1, 1),
                                                       Component::WallIdx::RIGHT}));

  m_boundaries[Component::WallIdx::BOTTOM] = std::move(Component::Wall<V>({-y_half,
                                                       V(0, 1, 0),
                                                       V(1, -1, 1),
                                                       Component::WallIdx::BOTTOM}));

  m_boundaries[Component::WallIdx::TOP]    = std::move(Component::Wall<V>({y_half,
                                                       V(0, -1, 0),
                                                       V(1, -1, 1),
                                                       Component::WallIdx::TOP}));

  m_boundaries[Component::WallIdx::BACK]   = std::move(Component::Wall<V>({-z_half,
                                                       V(0, 0, 1),
                                                       V(1, 1, -1),
                                                       Component::WallIdx::BACK}));

  m_boundaries[Component::WallIdx::FRONT]  = std::move(Component::Wall<V>({z_half,
                                                       V(0, 0, -1),
                                                       V(1, 1, -1),
                                                       Component::WallIdx::FRONT}));

}

template<typename V>
void SimulationContext<V>::set_free_run(bool free_run) {
  m_free_run = free_run;
}

template<typename V>
void SimulationContext<V>::set_settings(const SimSettings<vector_t>& settings) {
  m_settings = Util::LatchingValue<SimSettings<typename V::vector_t>>(settings);
}

template<typename V>
const SimSettings<typename V::vector_t>& SimulationContext<V>::get_settings() const {
  return m_settings.get();
}

template<typename V>
void SimulationContextThread(SimulationContext<V>& sim, SimSettings<typename V::vector_t> settings) {
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
        std::thread(Util::ring_thread<std::vector<Component::Particle<V>>,
                                      Component::Particle<V>,
                                      SimSettings<typename V::vector_t>::RingBufferSize>,
                                      std::ref(sim.m_particle_buffer));
  ring_buffer_copy_thread.detach();

  // make 2 copies of the simulation at the start.. allows for corrections if an error occurs on the first frame
  sim.m_particle_buffer.put();

  while(true) {
    if (!g_pause) {
      sim.run();
    } else {
      if (g_step) {
        sim.run();
        g_step = false;
      }
    }
  }
}

template class SimulationContext<Component::Vector<float>>;
template class SimulationContext<Component::Vector<double>>;
template class SimulationContext<Component::Vector<Util::FixedPoint>>;

template void SimulationContextThread(SimulationContext<Component::Vector<float>>& sim, SimSettings<float> settings);
template void SimulationContextThread(SimulationContext<Component::Vector<double>>& sim, SimSettings<double> settings);
template void SimulationContextThread(SimulationContext<Component::Vector<Util::FixedPoint>>& sim, SimSettings<Util::FixedPoint> settings);

} // namespace Simulation