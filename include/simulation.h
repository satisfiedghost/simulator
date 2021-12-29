#include "particle.h"
#include <vector>
#include <chrono>

namespace Simulation {

/**
 * The simulation class manages time and holds references to all objects within the context
 */
class SimulationContext {
public:
  SimulationContext() {}

  // get the time of the simulation
  std::chrono::microseconds get_simulation_time() const;

  // add a particle into the simulation
  // makes a copy of the particle
  void add_particle(Particle<float>);

  // add a particle into the simulation by passing a velocity and position vector
  void add_particle(const Vector<float>&, const Vector<float>&);

  // run the for x steps
  void run(size_t steps = 1);

private:
  std::vector<Particle<float>> m_particles;
  std::chrono::microseconds m_sim_time;
};

} // Simulation