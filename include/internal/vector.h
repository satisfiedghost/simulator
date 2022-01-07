namespace Component {

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

} // Component
