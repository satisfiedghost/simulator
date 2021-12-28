#include <cstdint>
#include <iostream>
#include <type_traits>

namespace Simulation {
template<typename T>
struct Vector {
  static_assert(!std::is_integral<T>::value, "Integral types are not yet supported.");

  Vector(T x, T y, T z);

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

//// define scalar operations
template<typename T>
Vector<T> operator*(const Vector<T>&, const T);

// scalar multiplication is commutative
template<typename T>
Vector<T> operator*(const T, const Vector<T>&);

template<typename T>
Vector<T> operator/(const Vector<T>&, const T);

// you can add or subtract two vectors
template<typename T>
Vector<T> operator+(const Vector<T>&, const Vector<T>&);

template<typename T>
Vector<T> operator-(const Vector<T>&, const Vector<T>&);

// define dot product
template<typename T>
T operator^(const Vector<T>&, const Vector<T>&);

// define cross product
template<typename T>
Vector<T> operator%(const Vector<T>&, const Vector<T>&);

// scalar multiplaction with other types
template<typename T, typename S>
Vector<T> operator*(const Vector<T>&, const S);

} // namespace Simulation

#include "vector.tpp"