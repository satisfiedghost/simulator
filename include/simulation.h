#include "particle.h"
#include <vector>
#include "sim_time.h"
#include <array>

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
                   , m_free_run(false)
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

  // create a simulation box, centered about the origin, with dimensions {x, y, z}
  void set_boundaries(float, float, float);

  // If true, run let the system free run
  // If false, run in steps of the time resolution
  void set_free_run(bool);

private:
  std::vector<Particle<float>> m_particles;
  chrono::steady_clock m_sim_clock;
  const chrono::time_point<chrono::steady_clock, US_T> start;
  chrono::time_point<chrono::steady_clock, US_T> m_tock;
  bool m_free_run;

  // simulation boundaries
  struct Wall {
    Wall();
    // 1D position of this wall, assumed to be a plane wrt normal
    float position;
    // Normal vector, points toward simulation origin
    Vector<float> normal;
    // When a collision on this wall occurs, multiply by this to get the new velocity
    Vector<float> inverse;
  };

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

  std::array<Wall, WallIdx::SIZE> m_boundaries;

  // Number of steps the simulator has run
  size_t m_step = 0;
};

} // Simulation