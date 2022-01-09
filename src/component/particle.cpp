#include "component.h"

namespace Component {

template<typename V>
void Particle<V>::set_velocity(const V& v) {
  m_velocity = v;
  m_kinetic_energy = calculate_kinetic_energy();
}

template<typename V>
void Particle<V>::set_position(const V& p) {
  m_position = p;
}

template class Particle<Vector<float>>;
template class Particle<Vector<double>>;
template class Particle<Vector<Util::FixedPoint>>;

} // namespace Simulation
