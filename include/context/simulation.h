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

template<typename T>
class SimulationContext {
public:
  // Construct a simulation with settings
  SimulationContext(SimSettings settings)
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
                   , m_settings(DefaultSettings)
                   {}

  // get the time of the simulation
  chrono::time_point<chrono::steady_clock> get_simulation_time() const;

  // get the elapsed time of the system
  chrono::microseconds get_elapsed_time_us() const;

  // add a particle into the simulation
  // makes a copy of the particle
  void add_particle(Component::Particle<T>);

  // add a particle into the simulation by passing a velocity and position vector
  void add_particle(const Component::Vector<T>&, const Component::Vector<T>&);

  // run the simulation, please call repeatedly in a dedicated thread
  void run();

  // read-only view of particles
  const std::vector<Component::Particle<T>>& get_particles() const {
    return m_particle_buffer.latest();
  }

  // create a simulation box, centered about the origin, with dimensions {x, y, z}
  // only support this as a whole number now (it's generally the size of the screen)
  void set_boundaries(size_t, size_t, size_t);

  // If true, run let the system free run
  // If false, run in steps of the time resolution
  void set_free_run(bool);

  // Set the settings
  void set_settings(const SimSettings&);

  // View the settings this simulation is using
  const SimSettings& get_settings() const;

  template<typename S>
  friend void SimulationContextThread(SimulationContext<S>& sim, SimSettings settings);

  void set_physics_context(Simulation::PhysicsContext<T> pc) {
    m_physics_context = pc;
    m_physics_context.set_sim(this);
  }

  size_t get_step() { return m_step; }

private:
  void add_particle_internal(Component::Particle<T>&);

  // keeping a ringbuffer of particles allows us to go back N steps in time, with minimal overhead
  Util::ThreadedRingBuffer<std::vector<Component::Particle<T>>, Component::Particle<T>, SimSettings::RingBufferSize> m_particle_buffer;

  chrono::steady_clock m_sim_clock;
  const chrono::time_point<chrono::steady_clock, US_T> m_start;
  chrono::time_point<chrono::steady_clock, US_T> m_tock;
  bool m_free_run;

  std::array<Component::Wall<T>, Component::WallIdx::SIZE> m_boundaries;

  // Number of steps the simulator has run
  size_t m_step = 0;

  // Number of times the simulator has detectd an impossible situation
  size_t m_impossible_count = 0;

  // Number of collisions experienced by system.
  size_t m_collision_count = 0;

  // Number of bounces experienced by system.
  size_t m_bounce_count = 0;

  // Whether we should calculate the next step in the simulation
  bool should_calc_next_step;

  // Total particles in system
  size_t m_particle_count = 0;

  // Invariant settings for the system (well as long as you don't call update settings at runtime, which might be fun)
  Util::LatchingValue<SimSettings> m_settings;

  // Physics rules for the simulation
  Simulation::PhysicsContext<T> m_physics_context;
};

// run me!
// run me!
// come on, run me!
template<typename T>
void SimulationContextThread(SimulationContext<T>& sim, SimSettings settings);

} // Simulation
