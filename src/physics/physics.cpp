#include "context.h"

namespace Simulation {

template<typename T>
Status PhysicsContext<T>::collide(Component::Particle<T>& a, Component::Particle<T>& b) {
  // are these particles even close enough for this?
  // define a vector from the other object's COM to ours
  const auto dist = a.get_position() - b.get_position();

  // nope too far away
  if (dist.magnitude > static_cast<T>(a.get_radius() + b.get_radius())) {
    return Status::None;
  }

  const auto va_before = a.get_velocity();
  const auto vb_before = b.get_velocity();
  const auto ka_before = a.get_kinetic_energy();
  const auto kb_before = b.get_kinetic_energy();

  // Got some work to do.
  const auto v_delta_before = (va_before - vb_before);

  // get the impulse unit vector
  const auto impulse_unit_vector = (dist).unit_vector();

  // the impulse vector is the dot product of its unit vector and the difference in velocities, multiplied by the unit vector
  // I am not completely sure why, but we seem to need to use the absolute value of the dot product lest it be negative and
  // flip the direction of our impulse
  const auto dot_product_abs = std::abs(impulse_unit_vector ^ v_delta_before);
  const auto impulse_vector = 2 * impulse_unit_vector * (dot_product_abs / (a.get_inverse_mass() + b.get_inverse_mass()));

// TODO use actual debug logging....
#ifdef DEBUG
  if (Simulation::trace_present(m_settings.get().trace, a.uid.get()) or
      Simulation::trace_present(m_settings.get().trace, b.uid.get())) {
    std::cout << "*******************Collision detected!*******************" << std::endl;
    std::cout << "*********************************************************" << std::endl;
    std::cout << "On Step: " << m_outer_sim->get_step() << std::endl;
    auto elapsed_time = m_outer_sim->get_elapsed_time_us().count();
    std::cout << "Elapsed Time: " << elapsed_time << "us " << "| ("
      << static_cast<float>(elapsed_time) / static_cast<float>(1e6) << "s)" << std::endl;
    std::cout << "Distance: " << dist << std::endl;
    std::cout << "V Delta: " << v_delta_before << std::endl;
    std::cout << "Particle A (Pre): " << std::endl << a << std::endl;
    std::cout << "Particle B (Pre): " << std::endl << b << std::endl;
    std::cout << "Total KE (Pre): " << ka_before + kb_before << std::endl << std::endl;
    std::cout << "Impulse unit vector: " << impulse_unit_vector << std::endl;
    std::cout << "Impulse vector: " << impulse_vector << std::endl;
  }
#endif

  // now that we have our impulse vector accounted for, we can calculate post-collision velocities
  // once again, this does not account for mass weighting yet
  a.set_velocity(a.get_velocity() + impulse_vector / a.get_mass());
  b.set_velocity(b.get_velocity() - impulse_vector / b.get_mass());

  // If a particular particle is moving too fast or lines up just right...
  // It can go through another particle before their collisions are detected.
  // This should be mitigated eventually, but for now detect it and bail out, restoring original velocities.
  // This results in a large gain in kinetic energy (is there a better way to detect this?)
  const auto ka_after = a.get_kinetic_energy();
  const auto kb_after = b.get_kinetic_energy();
  const auto k_delta = std::abs((ka_before + kb_before) - (ka_after + kb_after));

  if (k_delta > 1) {
    // nope
#ifdef DEBUG
  if (Simulation::trace_present(m_settings.get().trace, a.uid.get()) or
      Simulation::trace_present(m_settings.get().trace, b.uid.get())) {
    std::cout << "!!!!!!!!!!!!!!!!!!Detected impossible collision!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "Particle A (Would Have Been): " << std::endl << a << std::endl;
    std::cout << "Particle B (Would Have Been): " << std::endl << a << std::endl;
    std::cout << "Delta KE: " << k_delta << std::endl;
    std::cout << "Total KE (Would Have Been): " << ka_after + kb_after << std::endl;
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
    std::cout << "$$$$$$$$$$$$$$$$$$End Collision Data$$$$$$$$$$$$$$$$$$" << std::endl << std::endl;
  }
#endif
    a.set_velocity(va_before);
    b.set_velocity(va_before);
    return Status::Impossible;
  }
#ifdef DEBUG
  if (Simulation::trace_present(m_settings.get().trace, a.uid.get()) or
      Simulation::trace_present(m_settings.get().trace, b.uid.get())) {
  std::cout << "Particle A (Post): " << std::endl << a << std::endl;
  std::cout << "Particle B (Post): " << std::endl << b << std::endl;
  std::cout << "Delta KE: " << k_delta << std::endl;
  std::cout << "Total KE (Post): " << ka_after + kb_after << std::endl;
  std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
  std::cout << "$$$$$$$$$$$$$$$$$$End Collision Data$$$$$$$$$$$$$$$$$$" << std::endl << std::endl;
  }
#endif
  // that's it!
  return Status::Success;
}

template<typename T>
Status PhysicsContext<T>::bounce(Component::Particle<T>& p, const Component::Wall<T>& wall) {

  // are we traveling toward this wall?
  // velocity sign in relevant direction must oppose normal vector of wall
  auto v_relative = p.get_velocity() * wall.normal().abs();

  // since this is now a 1D vector, its sum is just the relevant portion... check if signs are opposite
  if ((v_relative.sum() < 0) == (wall.normal().sum() < 0)) {
    return Status::None;
  }

  // Ok we're traveling at the wall, but are we close enough for a collision?
  auto distance = std::abs((p.get_position() * wall.normal().abs()).sum() - wall.position());

  if (distance <= static_cast<T>(p.get_radius())) {
    p.set_velocity(p.get_velocity() * wall.inverse());
#ifdef DEBUG
  if (Simulation::trace_present(m_settings.get().trace, p.uid.get())) {
    std::cout << "*******************Bounce detected!*******************" << std::endl;
    std::cout << "******************************************************" << std::endl;
    std::cout << "On Step: " << m_outer_sim->get_step() << std::endl;
    auto elapsed_time = m_outer_sim->get_elapsed_time_us().count();
    std::cout << "Elapsed Time: " << elapsed_time << "us " << "| ("
      << static_cast<float>(elapsed_time) / static_cast<float>(1e6) << "s)" << std::endl;
    std::cout << "Wall: " << wall << std::endl;
    std::cout << "Particle (Post):" << std::endl << p << std::endl;
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl;
    std::cout << "$$$$$$$$$$$$$$$$$$End Bounce Data$$$$$$$$$$$$$$$$$$" << std::endl << std::endl;
  }
#endif
    return Status::Success;
  }
  return Status::None;
}

template<typename T>
void PhysicsContext<T>::gravity(Component::Particle<T>& p) {
  p.set_velocity(p.get_velocity() + m_gravity.get());
}

template<typename T>
void PhysicsContext<T>::step(Component::Particle<T>& p, US_T us) {
  // move the amount we would expect, with our given velocity
  T time_scalar = chrono::duration_cast<chrono::duration<T>>(us).count();
  p.set_position(p.get_position() + (p.get_velocity() * time_scalar));
}

template class PhysicsContext<float>;
template class PhysicsContext<double>;

} // namespace Physics
