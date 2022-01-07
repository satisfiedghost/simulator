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

template <typename T>
class PhysicsContext {
public:
  PhysicsContext() : m_settings(Simulation::DefaultSettings) {}

  PhysicsContext(Simulation::SimSettings settings)
                   : m_settings(settings)
                   , m_gravity(Component::Vector<T>(
                                m_settings.get().gravity * std::cos(static_cast<T>(M_PI) * m_settings.get().gravity_angle / 180.f),
                                m_settings.get().gravity * std::sin(static_cast<T>(M_PI) * m_settings.get().gravity_angle / 180.f),
                                0))
                  {}

  // Collide one particle into another
  // Status::None if no collision occurred (too far away)
  // Status::Success if a collision occurred
  // Status::Impossible if we attempted a collision, but the result was impossible.
  // O3 optimized because this is a hyper critical loop, and doing so results in
  // inlining all the vector operations, yielding a 5-10x performance increase overall
  #if defined(__clang__)
  #elif defined(__GNUC__)
  __attribute__((optimize(3)))
  #endif
  Status collide(Component::Particle<T>&, Component::Particle<T>&);

  // Bounce a particle off a wall.
  // Status::None if the particle did not bounce off the wall. Too far away, or not traveling toward it.
  // Status::Success if the particle bounced off the wall, and has a new velocity.
  Status bounce(Component::Particle<T>&, const Component::Wall<T>&);

  void set_sim(Simulation::SimulationContext<T>* sim) {
    m_outer_sim = sim;
  }

  // accelerate a particle in the direction of gravity
  void gravity(Component::Particle<T>&);

  // Move a particle forward in time.
  void step(Component::Particle<T>&, US_T);
private:
  Util::LatchingValue<Simulation::SimSettings> m_settings;

  Util::LatchingValue<Component::Vector<T>> m_gravity;
  // access to the simulation in which we're running
  // TODO manage ths properly, should be read-only
  Simulation::SimulationContext<T>* m_outer_sim;
};

} // Physics
