// Prevent users of particle.h from including these libs
#ifndef PARTICLE_TPP_INCLUDE
#include <cmath>
#endif
#define PARTICLE_TPP_INCLUDE

namespace Simulation {

template<typename T>
void Particle<T>::step(int64_t steps) {
  // since our velocity is defined as moving 1 TIME_UNIT_SIZE
  // per step... this is simple vector addition
  m_position = m_position + ((m_velocity * TIME_RESOLUTION)* steps);
}

template<typename T>
bool Particle<T>::collide(Particle<T>& other) {
  // are these particles even close enough for this?
  // define a vector from the other object's COM to ours
  auto dist = m_position - other.m_position;

  // nope too far away
  if (dist.magnitude > RADIUS * 2) {
    return false;
  }

// TODO use actual debug logging....
#ifdef DEBUG
  std::cout << "Collision detected!" << std::endl;
  std::cout << "This particle: " << std::endl << (*this) << std::endl;
  std::cout << "Other particle: " <<  std::endl << other << std::endl;
#endif

  // Got some work to do.
  auto v_diff = (m_velocity - other.m_velocity);

  // get the impulse unit vector
  auto impulse_unit = (dist).unit_vector();

  // TODO implement masses, for now everything is an equal mass of 1 unit
  // TODO have a README on where this comes from...

  // the impulse vector is the dot product of its unit vector and the difference in velocities, multiplied by the unit vector
  // I am not completely sure why, but we seem to need to use the absolute value of the dot product lest it be negative and
  // flip the direction of our impulse
  auto dot_product_abs = std::abs(impulse_unit ^ v_diff);
  std::cout << "dot product_abs: " << dot_product_abs << std::endl;
  auto impulse_vector = impulse_unit * (dot_product_abs);

  // now that we have our impulse vector accounted for, we can calculate post-collision velocities
  // once again, this does not account for mass weighting yet
  m_velocity = m_velocity + impulse_vector;
  other.m_velocity = other.m_velocity - impulse_vector;

#ifdef DEBUG
  std::cout << "Impulse unit vector: " << impulse_unit << std::endl;
  std::cout << "Impulse vector: " << impulse_vector << std::endl;
  std::cout << "Dist: " << dist << std::endl;
  std::cout << "Vdiff: " << v_diff << std::endl;
  std::cout << "Our new velocity: " << m_velocity << std::endl;
  std::cout << "Their new velocity: " << other.m_velocity << std::endl;
  std::cout << std::endl;
#endif
  // that's it!
  return true;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Particle<T>& p) {
  os << "V : " << p.m_velocity << std::endl;
  os << "P : " << p.m_position;
}

} // namespace Simulation