#include "particle.h"
#include <vector>
#include "sim_time.h"

namespace Simulation {

/**
 * The simulation class manages time and holds references to all objects within the context
 */
class SimulationContext {
public:
  SimulationContext()
                   : m_particles()
                   , m_sim_clock()
                   , start(chrono::time_point_cast<US_T>(m_sim_clock.now()))
                   , m_tock(start)
                   {}

  // get the time of the simulation
  chrono::time_point<chrono::steady_clock> get_simulation_time() const;

  // add a particle into the simulation
  // makes a copy of the particle
  void add_particle(Particle<float>);

  // add a particle into the simulation by passing a velocity and position vector
  void add_particle(const Vector<float>&, const Vector<float>&);

  // run the simulation, please call repeatedly in a dedicated thread
  void run();

  // read-only view of particles
  const std::vector<Particle<float>>& get_particles() const {return m_particles;};

private:
  std::vector<Particle<float>> m_particles;
  chrono::steady_clock m_sim_clock;
  const chrono::time_point<chrono::steady_clock, US_T> start;
  chrono::time_point<chrono::steady_clock, US_T> m_tock;
  size_t m_step = 0;
};

} // Simulation