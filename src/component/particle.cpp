#include "component.h"

namespace Component {

template<typename V>
void Particle<V>::set_velocity(const V& v, EnergyInvalidationPolicy ep) {
  m_velocity = v;
  if (ep == EnergyInvalidationPolicy::INVALIDATE) {
    this->is_kinetic_energy_valid = false;
  }
}

template<typename V>
void Particle<V>::set_position(const V& p) {
  m_position = p;
}

template<typename V>
const typename V::vector_t& Particle<V>::kinetic_energy() {
  if (!this->is_kinetic_energy_valid) {
    this->m_kinetic_energy = static_cast<vector_t>(0.5) * m_mass * pow(m_velocity.magnitude(), static_cast<vector_t>(2));
    this->is_kinetic_energy_valid = true;
  }
  return this->m_kinetic_energy;
}

template<typename V>
const typename V::vector_t& Particle<V>::inverse_mass() {
  if (!this->is_inverse_mass_valid) {
    this->m_inverse_mass = pow(this->m_mass, static_cast<vector_t>(-1));
    this->is_inverse_mass_valid = true;
  }
  return this->m_inverse_mass;
}

template class Particle<Vector<float>>;
template class Particle<Vector<double>>;
template class Particle<Vector<Util::FixedPoint>>;

} // namespace Simulation
