#include <cstdint>
#include "vector.h"
#include <iostream>
#include <type_traits>
#include "sim_time.h"
#include "latch.h"
#include "status.h"

namespace Simulation {

using std::size_t;

// Point particle
template<typename T>
class Particle {

static_assert(!std::is_integral<T>::value, "Integral types are not yet supported.");

public:

// radius of a unit. If the distance between 2 particles is lessr than this after a time step, they have collided
static constexpr size_t RADIUS = 10;

  // Defaults everything to 0
  Particle() : m_velocity(0, 0, 0)
             , m_position(0, 0, 0)
             {}

  // or supply some numbers of your own
  Particle(T xv, T yv, T zv,
           T xp, T yp, T zp)
           : m_velocity(xv, yv, zv)
           , m_position(xp, yp, zp)
           {}

  // Create a particle with vectors
  Particle(const Vector<T>& v, const Vector<T> &p)
           : m_velocity(v)
           , m_position(p)
           {}

  // moves the particle a total of some time
  // or supply a value to run the simulation forward or backward
  void step(US_T us);

  // collide 2 particles
  // if their distance is greater than RADIUS, then no effect.
  // otherwise, they collide
  // returns true if a collision occurred
  // O3 optimized because this is a hyper critical loop, and doing so results in
  // inlining all the vector operations, yielding a 5-10x performance increase overall

#if defined(__clang__)
#elif defined(__GNUC__)
  __attribute__((optimize(3)))
#endif
  Status collide(Particle<T>& other);

  // bounce off a normal vector e.g. the edge of the simulation
  void bounce(const Vector<T> &normal);

  // get a copy of this particle's position vector
  const Vector<T> get_position() const { return m_position; };

  // get a copy of this particle's velocity vector
  const Vector<T> get_velocity() const { return m_velocity; };

  // UID for this particle
  // 0 is an invalid UID
  LatchingValue<size_t> uid;

  template <typename S>
  friend std::ostream& operator<<(std::ostream &os, const Particle<S> &t);
private:
  // velocity
  Vector<T> m_velocity;
  // 3d position
  Vector<T> m_position;
};

} //namespace Simulation

#include "particle.tpp"

