// Prevent users of vector.h from including these libs
#ifndef VECTOR_TPP_INCLUDE
#include <cmath>
#endif
#define VECTOR_TPP_INCLUDE

namespace Simulation {

template<typename T>
Vector<T>::Vector(T x, T y, T z)
        : m_x(x)
        , m_y(y)
        , m_z(z)
        {
          magnitude = std::sqrt(
                      std::pow(m_x, 2) + 
                      std::pow(m_y, 2) +
                      std::pow(m_z, 2));
        }

template<typename T>
Vector<T> Vector<T>::unit_vector() const {
  return (*this) / this->magnitude;
}

template<typename T>
Vector<T> Vector<T>::abs() const {
  return Vector<T>(std::abs(this->m_x),
                   std::abs(this->m_y),
                   std::abs(this->m_z));
}

template<typename T>
Vector<T> Vector<T>::collinear_vector(T magnitude) const {
  T scalar = magnitude / this->magnitude;
  return scalar * (*this);
}

// todo should these be available as a member function to avoid copy overhead?
template<typename T>
Vector<T> operator*(const Vector<T>& v, const T s) {
  return Vector<T>(v.m_x * s,
                   v.m_y * s,
                   v.m_z * s);
}

template<typename T>
Vector<T> operator*(const T s, const Vector<T>& v) {
  return Vector<T>(v.m_x * s,
                v.m_y * s,
                v.m_z * s);
}

template<typename T>
Vector<T> operator/(const Vector<T>& v, const T s) {
  return Vector<T>(v.m_x / s,
                v.m_y / s,
                v.m_z / s);
}

// also should probably make these available as members
template<typename T>
Vector<T> operator+(const Vector<T>& first, const Vector<T>& second) {
  return Vector<T>(first.m_x + second.m_x,
                   first.m_y + second.m_y,
                   first.m_z + second.m_z);
}

template<typename T>
Vector<T> operator-(const Vector<T>& first, const Vector<T>& second) {
  return Vector<T>(first.m_x - second.m_x,
                   first.m_y - second.m_y,
                   first.m_z - second.m_z);
}

// dot product
template<typename T>
T operator^(const Vector<T>& first, const Vector<T>& second) {
  return first.m_x * second.m_x + 
         first.m_y * second.m_y +
         first.m_z * second.m_z;
}

// cross product
template<typename T>
T operator%(const Vector<T>& first, const Vector<T>& second) {
  // c1 = a2b3 - a3b2
  // c2 = a3b1 - a1b3
  // c3 = a1b2 - a2b1
  return Vector<T>(first.two() * second.three() - first.three() * second.two(),
                   first.three() * second.one() - first.one() * second.three(),
                   first.one() * second.two() - first.two() * second.one());
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& v) {
  os << "{" << v.m_x << " : " << v.m_y << " : " << v.m_z << " | " << v.magnitude << "}";
  return os;
}

template<typename T, typename S>
Vector<T> operator*(const Vector<T>& v, const S s) {
  return Vector<T>(v.m_x * s,
                   v.m_y * s,
                   v.m_z * s);
}


} // namespace Simulation