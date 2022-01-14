// Prevent users of vector.h from including these libs
#include "component.h"

namespace Component {

template<typename T, typename U>
Vector<T, U> Vector<T, U>::unit_vector() const {
  return (*this) / this->magnitude;
}

template<typename T, typename U>
Vector<T, U> Vector<T, U>::absolute() const {
  return Vector<T, U>(abs(this->m_x),
                      abs(this->m_y),
                      abs(this->m_z));
}

// I'm not sure if this has a mathematic usefulness...
// but it is useful for our bounce calculations
template<typename T, typename U>
T Vector<T, U>::sum() const {
  return (m_x +
          m_y +
          m_z);
}

template<typename T, typename U>
Vector<T, U> Vector<T, U>::collinear_vector(T magnitude) const {
  T scalar = magnitude / this->magnitude;
  return scalar * (*this);
}


template struct Vector<float>;
template struct Vector<double>;
template struct Vector<Util::FixedPoint>;
template struct Vector<Util::FixedPoint, double>;

} // namespace Simulation
