#pragma once
#include "component.h"
#include "status.h"

#include <cmath>

// Functions for physical interactions
namespace Physics {

// Collide one particle into another
// Status::None if no collision occurred (too far away)
// Status::Success if a collision occurred
// Status::Impossible if we attempted a collision, but the result was impossible.
// O3 optimized because this is a hyper critical loop, and doing so results in
// inlining all the vector operations, yielding a 5-10x performance increase overall
template<typename T>
#if defined(__clang__)
#elif defined(__GNUC__)
__attribute__((optimize(3)))
#endif
Status collide(Component::Particle<T>&, Component::Particle<T>&);

// Bounce a particle off a wall.
// Status::None if the particle did not bounce off the wall. Too far away, or not traveling toward it.
// Status::Success if the particle bounced off the wall, and has a new velocity.
template<typename T>
Status bounce(Component::Particle<T>&, const Component::Wall<T>&);

} // Physics
