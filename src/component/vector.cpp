// Prevent users of vector.h from including these libs
#include "component.h"

namespace Component {

template<typename T>
Vector<T> Vector<T>::unit_vector() const {
  return (*this) / const_cast<Vector<T>*>(this)->magnitude();
}

template<typename T>
Vector<T> Vector<T>::absolute() const {
  return Vector<T>(abs(this->m_x),
                   abs(this->m_y),
                   abs(this->m_z));
}

// I'm not sure if this has a mathematic usefulness...
// but it is useful for our bounce calculations
template<typename T>
T Vector<T>::sum() const {
  return (m_x +
          m_y +
          m_z);
}

template<typename T>
Vector<T> Vector<T>::collinear_vector(T mag) const {
  T scalar = mag / const_cast<Vector<T>*>(this)->magnitude();
  return scalar * (*this);
}

template<typename T>
const T& Vector<T>::magnitude() {
  if (!this->is_mag_valid) {
    this->m_magnitude = sqrt(pow(m_x, static_cast<T>(2)) +
                             pow(m_y, static_cast<T>(2)) +
                             pow(m_z, static_cast<T>(2)));
    this->is_mag_valid = true;
  }
  return m_magnitude;
}

template class Vector<float>;
template class Vector<double>;
template class Vector<Util::FixedPoint>;

} // namespace Simulation
