// Prevent users of particle.h from including these libs
#ifndef PARTICLE_TPP_INCLUDE
#include <cmath>
#endif
#define PARTICLE_TPP_INCLUDE

namespace Simulation {

template<typename T>
void Particle<T>::step(US_T us) {
  // move the amount we would expect, with our given velocity
  T time_scalar = chrono::duration_cast<chrono::duration<T>>(us).count();

  m_position = m_position + (m_velocity * time_scalar);
}

template<typename T>
Status Particle<T>::collide(Particle<T>& other) {
  // are these particles even close enough for this?
  // define a vector from the other object's COM to ours
  auto dist = m_position - other.m_position;

  // nope too far away
  if (dist.magnitude > RADIUS * 2) {
    return Status::None;
  }

  auto ker = [](T v) {
    return std::pow(v, 2);
  };
  auto our_ker_before = ker(m_velocity.magnitude);
  auto their_ker_before = ker(other.m_velocity.magnitude);
  auto our_vel_before = m_velocity;
  auto their_vel_before = other.m_velocity;

// TODO use actual debug logging....
#ifdef DEBUG
  auto this_ker = std::pow((*this).m_velocity.magnitude, 2);
  auto other_ker = std::pow(other.m_velocity.magnitude, 2);
  auto total_pre_ker = this_ker + other_ker;
  std::cout << "Collision detected!" << std::endl;
  std::cout << "This particle: " << std::endl << (*this) << std::endl;;
  std::cout << "This KER: " << this_ker << std::endl;
  std::cout << "Other particle: " <<  std::endl << other << std::endl;
  std::cout << "Other KER: " << other_ker << std::endl;
  std::cout << "Total KER: " << total_pre_ker << std::endl;
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
  auto impulse_vector = impulse_unit * (dot_product_abs);

  // now that we have our impulse vector accounted for, we can calculate post-collision velocities
  // once again, this does not account for mass weighting yet
  m_velocity = m_velocity + impulse_vector;
  other.m_velocity = other.m_velocity - impulse_vector;

  // If a particular particle is moving too fast or lines up just right...
  // It can go through another particle before their collisions are detected.
  // This should be mitigated eventually, but for now detect it and bail out, restoring original velocities.
  // This results in a large gain in kinetic energy (is there a better way to detect this?)
  auto our_ker_after = ker(m_velocity.magnitude);
  auto their_ker_after = ker(other.m_velocity.magnitude);
  if (std::abs((our_ker_before + their_ker_before) - (our_ker_after + their_ker_after)) > 1) {
    // nope
    m_velocity = our_vel_before;
    other.m_velocity = their_vel_before;
    return Status::Impossible;
  }

#ifdef DEBUG
  this_ker = std::pow((*this).m_velocity.magnitude, 2);
  other_ker = std::pow(other.m_velocity.magnitude, 2);
  std::cout << "Impulse unit vector: " << impulse_unit << std::endl;
  std::cout << "Impulse vector: " << impulse_vector << std::endl;
  std::cout << "Dist: " << dist << std::endl;
  std::cout << "Vdiff: " << v_diff << std::endl;
  std::cout << "This particle now: " << (*this) << std::endl;
  std::cout << "This new KER: " << this_ker << std::endl;
  std::cout << "Other particle now: " << other << std::endl;
  std::cout << "Other new KER: " << other_ker << std::endl;
  std::cout << "Total new KER: " << this_ker + other_ker << std::endl;
  std::cout << "Diff KER: " << (this_ker + other_ker) - total_pre_ker << std::endl;
  std::cout << std::endl;
#endif
  // that's it!
  return Status::Success;
}

template<typename T>
void Particle<T>::bounce (const Vector<T>& inverse) {
  m_velocity = m_velocity * inverse;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Particle<T>& p) {
  os << "UID: " << p.uid.get() << std::endl;
  os << "V : " << p.m_velocity << std::endl;
  os << "P : " << p.m_position;
  return os;
}

} // namespace Simulation