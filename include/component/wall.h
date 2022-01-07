#pragma once
#include "vector.h"

#include <iostream>
#include <map>
#include <string>

namespace Component {

// convenient way to remember which wall is which
enum WallIdx {
  LEFT = 0,
  BACK = 1,
  RIGHT = 2,
  FRONT = 3,
  TOP = 4,
  BOTTOM = 5,
  SIZE = 6,
};

// A simulation boundary. The simulation is bounded by a rectangular prism
template<typename T>
class Wall {
public:
  Wall()
        : position_n(0)
        , normal_vector()
        , inverse_vector()
        , idx(WallIdx::SIZE)
        {}

  Wall(T p, Component::Vector<T> n, Component::Vector<T> i, WallIdx index)
        : position_n(p)
        , normal_vector(n)
        , inverse_vector(i)
        , idx(index)
        {}

  T position() const { return position_n; }
  const Component::Vector<T>& normal() const { return normal_vector; }
  const Component::Vector<T>& inverse() const { return inverse_vector; }


  template <typename S>
  friend std::ostream& operator<<(std::ostream &os, const Wall<S>&);
private:
  // 1D position of this wall, assumed to be a plane wrt normal
  T position_n;
  // Normal vector, points toward simulation origin
  Component::Vector<T> normal_vector;
  // When a collision on this wall occurs, multiply by this to get the new velocity
  Component::Vector<T> inverse_vector;
  // Which wall are we?
  WallIdx idx;
};


static std::map<size_t, std::string> wall_idx_to_str = {
  {WallIdx::LEFT, "Left"},
  {WallIdx::BACK, "Back"},
  {WallIdx::RIGHT, "Right"},
  {WallIdx::FRONT, "Front"},
  {WallIdx::TOP, "Top"},
  {WallIdx::BOTTOM, "Bottom"}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Wall<T>& wall) {
  os << "WallIdx: " << wall_idx_to_str[wall.idx] << " | Pos: " << wall.position_n << std::endl;
  os << "Normal: " << wall.normal_vector << std::endl;
  os << "Inverse: " << wall.inverse_vector;
  return os;
}

} // namespace Component
