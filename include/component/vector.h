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
template<typename T>
class Vector {
public:

  // Underlying type for the vector
  typedef T vector_t;

  Vector() {};

  Vector(T x, T y, T z)
        : m_x(x)
        , m_y(y)
        , m_z(z)
        , is_mag_valid(false)
        , m_magnitude(0)
        {}

  Vector(const Vector<T>& other) {
    this->m_x = other.m_x;
    this->m_y = other.m_y;
    this->m_z = other.m_z;
    this->is_mag_valid = other.is_mag_valid;
    this->m_magnitude = other.m_magnitude;
  }

  // get a unit vector
  Vector unit_vector() const;

  // Get this vector as an absolute vector
  // useful for e.g. "dividing" something across this vector without
  // worrying about signs
  Vector absolute() const;

  // Return a collinear vector with the given magnitude
  Vector collinear_vector(T) const;

  // Sum the components ove a vector
  T sum() const;

  // Alias for below, more clear what's going on in some contexts
  const T& x() const { return m_x; }
  const T& y() const { return m_y; }
  const T& z() const { return m_z; }

  // Convenient access notation if we aren't concerned about euclidian space
  const T& one() const {return m_x;};
  const T& two() const {return m_y;};
  const T& three() const {return m_z;};

  // Here's the tricky one... we often times do not need to calculate the magnitude of a vector and waste many CPU cycles doing it.
  // e.g. if a Particle moves thru space due to momentum but experiences no collisions nor bounces, we never access its distance
  // from origin and if we bother to compute it, we have uselessly run a square root, and several exponentiation routines
  // This accessor either returns an already calculated magnitude, otherwise it calculates it on-demand.
  const T& magnitude();

private:
  T m_x;
  T m_y;
  T m_z;

  bool is_mag_valid;
  T m_magnitude;
};

// TODO provide these as member functions

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
