// Prevent users of particle.h from including these libs
#ifndef PARTICLE_TPP_INCLUDE
#include <cmath>
#endif
#define PARTICLE_TPP_INCLUDE

namespace Component {

template<typename T>
void Particle<T>::step(US_T us) {
  // move the amount we would expect, with our given velocity
  T time_scalar = chrono::duration_cast<chrono::duration<T>>(us).count();

  m_position = m_position + (m_velocity * time_scalar);
}

template<typename T>
void Particle<T>::set_velocity(const Vector<T>& v) {
  m_velocity = v;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Particle<T>& p) {
  os << "{{UID: " << p.uid.get()  << ", Radius: " << p.get_radius() << std::endl;
  os << "  Mass: " << p.m_mass << " KE: " << p.m_kinetic_energy << std::endl;
  os << "  Vel : " << p.m_velocity << std::endl;
  os << "  Pos : " << p.m_position << "}}";
  return os;
}

template<typename T>
bool operator==(const Particle<T>& first, const Particle<T>& second) {
  return first.uid.get() == second.uid.get() and
         first.get_mass() == second.get_mass() and
         first.get_radius() == second.get_radius() and
         first.get_velocity() == second.get_velocity() and
         first.get_position() == second.get_position();
}

} // namespace Simulation