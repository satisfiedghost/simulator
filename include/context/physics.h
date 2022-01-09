#pragma once
#include "component.h"
#include "context.h"
#include "sim_settings.h"
#include "util/latch.h"
#include "util/status.h"

#include <cmath>
#include <memory>

// Functions for physical interactions
namespace Simulation {

// Allows us to use ADL to defer to the correct sqrt/pow impls
using std::cos;
using std::sin;
using std::abs;

template <typename V>
class PhysicsContext {

typedef typename V::vector_t vector_t;

public:
  PhysicsContext() : m_settings(Simulation::DefaultSettings<vector_t>) {}

  PhysicsContext(Simulation::SimSettings<vector_t> settings)
                   : m_settings(settings)
                   , m_gravity(V(m_settings.get().gravity *
                                   cos(static_cast<vector_t>(M_PI) *
                                     static_cast<vector_t>(m_settings.get().gravity_angle) / static_cast<vector_t>(180)),
                                 m_settings.get().gravity *
                                   sin(static_cast<vector_t>(M_PI) *
                                     static_cast<vector_t>(m_settings.get().gravity_angle) / static_cast<vector_t>(180)),
                                 0))
                    ,in_replay_mode(false)
                  {}

  // Collide one particle into another
  // Status::None if no collision occurred (too far away)
  // Status::Success if a collision occurred
  // Status::Corrected if we encountered an error but were able to correct for it.
  // Status::Inconsistent if we encountered an error but were unable to correct for it.
  // O3 optimized because this is a hyper critical loop, and doing so results in
  // inlining all the vector operations, yielding a 5-10x performance increase overall
  #if defined(__clang__)
  #elif defined(__GNUC__)
  __attribute__((optimize(3)))
  #endif
  Status collide(Component::Particle<V>&, Component::Particle<V>&);

  // Bounce a particle off a wall.
  // Status::None if the particle did not bounce off the wall. Too far away, or not traveling toward it.
  // Status::Success if the particle bounced off the wall, and has a new velocity.
  Status bounce(Component::Particle<V>&, const Component::Wall<V>&);

  void set_sim(Simulation::SimulationContext<V>* sim) {
    m_outer_sim = sim;
  }

  // accelerate a particle in the direction of gravity
  void gravity(Component::Particle<V>&, US_T);

  // Move a particle forward in time.
  void step(Component::Particle<V>&, US_T);
private:
  // when an impossible collision is detected (result of clipping), we attempt to correct
  Status correct_collision(Component::Particle<V>&, Component::Particle<V>&);

  // allows 1 level of recursion
  Status collide_internal(Component::Particle<V>&, Component::Particle<V>&, bool);

  Util::LatchingValue<Simulation::SimSettings<vector_t>> m_settings;

  Util::LatchingValue<V> m_gravity;
  // access to the simulation in which we're running
  // TODO manage ths properly, should be read-only
  Simulation::SimulationContext<V>* m_outer_sim;

  // Don't echo events during replay, when correcting issues.
  bool in_replay_mode;
};

} // Physics
