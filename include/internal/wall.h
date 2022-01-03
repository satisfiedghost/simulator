#pragma once
#include "vector.h"

namespace Component {
// A simulation boundary. The simulation is bounded by a rectangular prism
template<typename T>
class Wall {
public:
  Wall()
        : position_n(0.f)
        , normal_vector()
        , inverse_vector()
        {}

  Wall(float p, Component::Vector<T> n, Component::Vector<T> i)
        : position_n(p)
        , normal_vector(n)
        , inverse_vector(i)
        {}

  float position() const { return position_n; }
  const Component::Vector<float>& normal() const { return normal_vector; }
  const Component::Vector<float>& inverse() const { return inverse_vector; }
private:
  // 1D position of this wall, assumed to be a plane wrt normal
  float position_n;
  // Normal vector, points toward simulation origin
  Component::Vector<float> normal_vector;
  // When a collision on this wall occurs, multiply by this to get the new velocity
  Component::Vector<float> inverse_vector;
};

} // namespace Component