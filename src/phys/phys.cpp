#include "phys.h"

namespace Physics {

template<typename T>
Status collide(Component::Particle<T>& a, Component::Particle<T>& b) {
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
  std::cout << "*******************Collision detected!*******************" << std::endl;
  std::cout << "Distance: " << dist << std::endl;
  std::cout << "V Delta: " << v_delta_before << std::endl;
  std::cout << "Particle A (Pre): " << std::endl << a << std::endl;
  std::cout << "Particle B (Pre): " << std::endl << b << std::endl;
  std::cout << "Total KE (Pre): " << ka_before + kb_before << std::endl << std::endl;
  std::cout << "Impulse unit vector: " << impulse_unit_vector << std::endl;
  std::cout << "Impulse vector: " << impulse_vector << std::endl;
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
    std::cout << "!!!!!!!!!!!!!!!!!!Detected impossible collision!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "Particle A (Would Have Been): " << std::endl << a << std::endl;
    std::cout << "Particle B (Would Have Been): " << std::endl << a << std::endl;
    std::cout << "Delta KE: " << k_delta << std::endl;
    std::cout << "Total KE (Would Have Been): " << ka_after + kb_after << std::endl;
    std::cout << "^^^^^^^^^^^^^^^^^^End Collision Data^^^^^^^^^^^^^^^^^^" << std::endl << std::endl;
#endif
    a.set_velocity(va_before);
    b.set_velocity(va_before);
    return Status::Impossible;
  }
#ifdef DEBUG
  std::cout << "Particle A (Post): " << std::endl << a << std::endl;
  std::cout << "Particle B (Post): " << std::endl << b << std::endl;
  std::cout << "Delta KE: " << k_delta << std::endl;
  std::cout << "Total KE (Post): " << ka_after + kb_after << std::endl;
  std::cout << "^^^^^^^^^^^^^^^^^^End Collision Data^^^^^^^^^^^^^^^^^^" << std::endl << std::endl;
#endif
  // that's it!
  return Status::Success;
}

template<typename T>
Status bounce(Component::Particle<T>& p, const Component::Wall<T>& wall) {
  // can't bounce off a wall if you're not traveling toward it....
  auto v_relative = (p.get_velocity() * wall.normal().abs());

  if (v_relative * wall.normal() == v_relative) {
    return Status::None;
  }

  // convert to a 1D position
  auto pos_relative = (p.get_position() * wall.normal().abs()).magnitude;

  if (std::abs(pos_relative - wall.position()) <= static_cast<T>(p.get_radius())) {
    p.set_velocity(p.get_velocity() * wall.inverse());
    return Status::Success;
  }
  return Status::None;
}

template Status bounce(Component::Particle<float>&, const Component::Wall<float>&);
template Status collide(Component::Particle<float>&, Component::Particle<float>&);

template Status bounce(Component::Particle<double>&, const Component::Wall<double>&);
template Status collide(Component::Particle<double>&, Component::Particle<double>&);

} // namespace Physics