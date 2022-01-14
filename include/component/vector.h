#pragma once

#include "util/fixed_point.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace Component {
// Allows us to use ADL to defer to the correct sqrt/pow impls
using std::sqrt;
using std::pow;
using std::abs;

// T is the underlying type
// U is an optional type which can be coverted to T
template<typename T, typename U = T>
struct Vector {

  // Undderlying type for the vector
  typedef T vector_t;

  Vector() {};

  Vector<T, U>(U x, U y, U z)
        : m_x(static_cast<T>(x))
        , m_y(static_cast<T>(y))
        , m_z(static_cast<T>(z))
        {
          magnitude = sqrt(pow(m_x, static_cast<T>(2)) +
                           pow(m_y, static_cast<T>(2)) +
                           pow(m_z, static_cast<T>(2)));
        }

  Vector(const Vector<T>& other) {
    this->m_x = other.m_x;
    this->m_y = other.m_y;
    this->m_z = other.m_z;
    this->magnitude = other.magnitude;
  }

  // get a unit vector
  Vector unit_vector() const;

  // Get this vector as an absolute vector
  // useful for e.g. "dividing" something across this vector without
  // worrying about signs
  Vector absolute() const;

  // Return a collinear vector with the given magnitude
  Vector collinear_vector(T magnitude) const;

  // Sum the components ove a vector
  T sum() const;

  // Convenient access notation if we aren't concerned about euclidian space
  const T& one() const {return m_x;};
  const T& two() const {return m_y;};
  const T& three() const {return m_z;};

  // Vector<T, U> is implicitly convertible to Vector<T>
  operator Vector<T>() { return Vector<T>(m_x, m_y, m_z); }

  T m_x;
  T m_y;
  T m_z;
  T magnitude;
};

// specialized int64 ctor
template <>
Vector<int64_t, double>::Vector(double x, double y, double z) {
  m_x = static_cast<int64_t>(x * 10'000UL);
  m_y = static_cast<int64_t>(y * 10'000UL);
  m_z = static_cast<int64_t>(z * 10'000UL);
}


// todo should these be available as a member function to avoid copy overhead?

// Vector * Scalar
template<typename T>
Vector<T> operator*(const Vector<T>& v, const T s);

template<typename T>
Vector<T> operator*(const T s, const Vector<T>& v);

// Vector / Scalar
template<typename T>
Vector<T> operator/(const Vector<T>& v, const T s);

// also should probably make these available as members
// Vector + Vector
template<typename T>
Vector<T> operator+(const Vector<T>& first, const Vector<T>& second);

// Vector - Vector
template<typename T>
Vector<T> operator-(const Vector<T>& first, const Vector<T>& second);

// Vector . Vector - dot product
template<typename T>
T operator^(const Vector<T>& first, const Vector<T>& second);

// Vector X Vector - cross product
template<typename T>
T operator%(const Vector<T>& first, const Vector<T>& second);

// How to print a vector
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& v);

// Vector * Scalar when S::type != V::type
template<typename T, typename S>
Vector<T> operator*(const Vector<T>& v, const S s);

template<typename T, typename S>
Vector<T> operator*(const S s, const Vector<T>& v);

// Vector * Vector
template<typename T>
Vector<T> operator*(const Vector<T>& first, const Vector<T>& second);

// Vector == Vector
template<typename T>
bool operator==(const Vector<T>& first, const Vector<T>& second);

} // namespace Component

// Define above without cluttering header
#include "internal/vector.h"
