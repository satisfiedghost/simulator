// Prevent users of vector.h from including these libs
#include "component.h"

namespace Component {

template<typename T>
Vector<T> Vector<T>::unit_vector() const {
  return (*this) / this->magnitude;
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
Vector<T> Vector<T>::collinear_vector(T magnitude) const {
  T scalar = magnitude / this->magnitude;
  return scalar * (*this);
}


template struct Vector<float>;
template struct Vector<double>;
template struct Vector<Util::FixedPoint>;

} // namespace Simulation
