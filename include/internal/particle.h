namespace Component {

template<typename V>
bool operator==(const Particle<V>& first, const Particle<V>& second) {
  return first.uid.get() == second.uid.get() and
         first.get_mass() == second.get_mass() and
         first.get_radius() == second.get_radius() and
         first.get_velocity() == second.get_velocity() and
         first.get_position() == second.get_position();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Particle<T>& p) {
  os << "{{UID: " << p.uid.get()  << ", Radius: " << p.get_radius() << std::endl;
  os << "  Mass: " << p.m_mass << " | KE: " << p.m_kinetic_energy << std::endl;
  os << "  Vel : " << p.m_velocity << std::endl;
  os << "  Pos : " << p.m_position << "}}";
  return os;
}

} // Component