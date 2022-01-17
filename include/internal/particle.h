namespace Component {

template<typename V>
bool operator==(const Particle<V>& first, const Particle<V>& second) {
  return first.uid.get() == second.uid.get() and
         first.mass() == second.mass() and
         first.radius() == second.radius() and
         first.velocity() == second.velocity() and
         first.position() == second.position();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Particle<T>& p) {
  // Note: Printing will force calculations of all on-demand members
  os << "{{UID: " << p.uid.get()  << ", Radius: " << p.radius() << std::endl;
  os << "  Mass: " << p.m_mass << " | KE: " << p.kinetic_energy() << std::endl;
  os << "  Vel : " << p.m_velocity << std::endl;
  os << "  Pos : " << p.m_position << "}}";
  return os;
}

} // Component