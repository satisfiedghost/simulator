#pragma once
#include "component.h"
#include "context.h"
#include "sim_settings.h"
#include "sim_time.h"
#include "util/ring_buffer.h"

#include <array>
#include <memory>
#include <tuple>
#include <vector>

namespace Simulation {
/**
 * The simulation class manages time and holds references to all objects within the context
 */

template<typename V>
class SimulationContext {

typedef typename V::vector_t vector_t;

public:
  // Construct a simulation with settings
  SimulationContext(SimSettings<vector_t> settings)
                    : SimulationContext() {
                      m_settings = settings;
                    }

  // Or do it later.
  SimulationContext()
                   : m_particle_buffer()
                   , m_sim_clock()
                   , m_start(chrono::time_point_cast<US_T>(m_sim_clock.now()))
                   , m_tock(m_start)
                   , m_free_run(false)
                   , should_calc_next_step(true)
                   , m_settings(DefaultSettings<vector_t>)
                   {}

  // get the time of the simulation
  chrono::time_point<chrono::steady_clock> get_simulation_time() const;

  // get the elapsed time of the system (real e.g. wall clock)
  chrono::microseconds get_elapsed_time_us() const;

  // add a particle into the simulation
  // makes a copy of the particle
  void add_particle(Component::Particle<V>);

  // add a particle into the simulation by passing a velocity and position vector
  void add_particle(const V&, const V&);

  // run the simulation, please call repeatedly in a dedicated thread
  void run();

  // read-only view of particles, in their last good state
  const std::vector<Component::Particle<V>>& get_particles() const {
    return m_particle_buffer.latest();
  }

  // create a simulation box, centered about the origin, with dimensions {x, y, z}
  // only support this as a whole number now (it's generally the size of the screen)
  void set_boundaries(size_t, size_t, size_t);

  const std::array<Component::Wall<V>, Component::WallIdx::SIZE>& get_boundaries() const {
    return m_boundaries;
  }

  // If true, run let the system free run
  // If false, run in steps of the time resolution
  void set_free_run(bool);

  // Set the settings
  void set_settings(const SimSettings<vector_t>&);

  // View the settings this simulation is using
  const SimSettings<vector_t>& get_settings() const;

  template<typename Vv>
  friend void SimulationContextThread(SimulationContext<Vv>& sim, SimSettings<typename Vv::vector_t> settings);

  void set_physics_context(Simulation::PhysicsContext<V> pc) {
    m_physics_context = pc;
    m_physics_context.set_sim(this);
  }

  size_t get_step() { return m_step; }

private:
  void add_particle_internal(Component::Particle<V>&);

  // keeping a ringbuffer of particles allows us to go back N steps in time, with minimal overhead
  Util::ThreadedRingBuffer<std::vector<Component::Particle<V>>, Component::Particle<V>, SimSettings<V>::RingBufferSize> m_particle_buffer;

  chrono::steady_clock m_sim_clock;
  const chrono::time_point<chrono::steady_clock, US_T> m_start;
  chrono::time_point<chrono::steady_clock, US_T> m_tock;
  bool m_free_run;

  std::array<Component::Wall<V>, Component::WallIdx::SIZE> m_boundaries;

  // Number of steps the simulator has run
  size_t m_step = 0;

  // Number of times the simulator detectd an impossible situation, and was able to mitigate it.
  size_t m_correction_count = 0;

  // Number of times the simulator detectd an impossible situation, and was not able to correct it.
  size_t m_inconsistent_count = 0;

  // Number of collisions experienced by system.
  size_t m_collision_count = 0;

  // Number of bounces experienced by system.
  size_t m_bounce_count = 0;

  // Whether we should calculate the next step in the simulation
  bool should_calc_next_step;

  // Total particles in system
  size_t m_particle_count = 0;

  // Invariant settings for the system (well as long as you don't call update settings at runtime, which might be fun)
  Util::LatchingValue<SimSettings<vector_t>> m_settings;

  // Physics rules for the simulation
  Simulation::PhysicsContext<V> m_physics_context;
};

// run me!
// run me!
// come on, run me!
template<typename V>
void SimulationContextThread(SimulationContext<V>& sim, SimSettings<typename V::vector_t> settings);

} // Simulation
