#include "component.h"

namespace Component {

template<typename T>
void Particle<T>::set_velocity(const Vector<T>& v) {
  m_velocity = v;
}

template<typename T>
void Particle<T>::set_position(const Vector<T>& p) {
  m_position = p;
}

template class Particle<float>;
template class Particle<double>;

} // namespace Simulation
