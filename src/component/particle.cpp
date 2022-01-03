// Prevent users of particle.h from including these libs
#include "component.h"

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

template class Particle<float>;
template class Particle<double>;

} // namespace Simulation
