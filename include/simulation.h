#pragma once
#include "component.h"
#include "sim_settings.h"
#include "sim_time.h"

#include <array>
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
                   : m_working_particles()
                   , m_prepared_particles()
                   , m_sim_clock()
                   , start(chrono::time_point_cast<US_T>(m_sim_clock.now()))
                   , m_tock(start)
                   , m_free_run(false)
                   , should_calc_next_step(true)
                   , m_settings(DefaultSettings)
                   {}

  // get the time of the simulation
  chrono::time_point<chrono::steady_clock> get_simulation_time() const;

  // add a particle into the simulation
  // makes a copy of the particle
  void add_particle(Component::Particle<T>);

  // add a particle into the simulation by passing a velocity and position vector
  void add_particle(const Component::Vector<T>&, const Component::Vector<T>&);

  // run the simulation, please call repeatedly in a dedicated thread
  void run();

  // read-only view of particles
  const std::vector<Component::Particle<T>>& get_particles() const {return m_prepared_particles;};

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

private:
  void add_particle_internal(Component::Particle<T>&);

  // the working particles are those actively being used to calculate the next state, and
  // have no guaratneed state
  std::vector<Component::Particle<T>> m_working_particles;

  // this is the copy clients recerive, which is always guaranteed to be in a valid state
  std::vector<Component::Particle<T>> m_prepared_particles;

  chrono::steady_clock m_sim_clock;
  const chrono::time_point<chrono::steady_clock, US_T> start;
  chrono::time_point<chrono::steady_clock, US_T> m_tock;
  bool m_free_run;

  // convenient way to remember which wall is which
  enum WallIdx {
    LEFT = 0,
    BACK = 1,
    RIGHT = 2,
    FRONT = 3,
    TOP = 4,
    BOTTOM = 5,
    SIZE = 6,
  };

  std::array<Component::Wall<T>, WallIdx::SIZE> m_boundaries;

  // Number of steps the simulator has run
  size_t m_step = 0;

  // Number of times the simulator has detectd an impossible situation
  size_t m_impossible_count = 0;

  // Whether we should calculate the next step in the simulation
  bool should_calc_next_step;

  // Invariant settings for the system (well as long as you don't call update settings at runtime, which might be fun)
  LatchingValue<SimSettings> m_settings;
};

// run me!
// run me!
// come on, run me!
template<typename T>
void SimulationContextThread(SimulationContext<T>& sim, SimSettings settings);

} // Simulation
