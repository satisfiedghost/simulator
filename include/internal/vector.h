#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace Component {

template<typename T>
struct Vector {
  static_assert(!std::is_integral<T>::value, "Integral types are not yet supported.");

  Vector() {};

  Vector(T x, T y, T z)
        : m_x(x)
        , m_y(y)
        , m_z(z)
        {
          magnitude = std::sqrt(std::pow(m_x, static_cast<T>(2)) +
                                std::pow(m_y, static_cast<T>(2)) +
                                std::pow(m_z, static_cast<T>(2)));
        }

  Vector(const Vector<T>& other) {
    this->m_x = other.m_x;
    this->m_y = other.m_y;
    this->m_z = other.m_z;
    this->magnitude = other.magnitude;
  }

  // get a unit vector
  Vector unit_vector() const;

  // Get this vector as a an absolute vector
  // useful for e.g. "dividing" something across this vector without
  // worrying about signs
  Vector abs() const;

  // Return a collinear vector with the given magnitude
  Vector collinear_vector(T magnitude) const;

  // Convenient access notation if we aren't concerned about euclidian space
  const T& one() const {return m_x;};
  const T& two() const {return m_y;};
  const T& three() const {return m_z;};

  T m_x;
  T m_y;
  T m_z;
  T magnitude;
};

// todo should these be available as a member function to avoid copy overhead?
template<typename T>
Vector<T> operator*(const Vector<T>& v, const T s) {
  return Vector<T>(v.m_x * s,
                   v.m_y * s,
                   v.m_z * s);
}

template<typename T>
Vector<T> operator*(const T s, const Vector<T>& v) {
  return v * s;
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
  return Vector<T>(v.m_x * static_cast<T>(s),
                   v.m_y * static_cast<T>(s),
                   v.m_z * static_cast<T>(s));
}

template<typename T, typename S>
Vector<T> operator*(const S s, const Vector<T>& v) {
  return v * s;
}

// multiply two vectors
template<typename T>
Vector<T> operator*(const Vector<T>& first, const Vector<T>& second) {
  return Vector<T>(first.one() * second.one(),
                   first.two() * second.two(),
                   first.three() * second.three());
}

template<typename T>
bool operator==(const Vector<T>& first, const Vector<T>& second) {
  return first.one() == second.one() and
         first.two() == second.two() and
         first.three() == second.three();
}

} // namespace Component
