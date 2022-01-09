#include "component.h"

namespace Component {

template<typename T>
void Particle<T>::set_velocity(const Vector<T>& v) {
  m_velocity = v;
  m_kinetic_energy = calculate_kinetic_energy(m_mass, v);
}

template<typename T>
void Particle<T>::set_position(const Vector<T>& p) {
  m_position = p;
}

template class Particle<float>;
template class Particle<double>;
template class Particle<int64_t>;

} // namespace Simulation
