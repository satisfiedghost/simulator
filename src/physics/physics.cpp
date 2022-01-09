#include "context.h"
#include "debug.h"

#include <array>
#include <tuple>

namespace Simulation {

template<typename V>
Status PhysicsContext<V>::collide(Component::Particle<V>& a, Component::Particle<V>& b) {
  return collide_internal(a, b, true);
}

template<typename V>
Status PhysicsContext<V>::collide_internal(Component::Particle<V>& a, Component::Particle<V>& b, bool retry) {
  // are these particles even close enough for this?
  // define a vector from the other object's COM to ours

  auto min_dist = a.get_radius() + b.get_radius();

  // TODO TODO TODO
  // THIS OBVIOUSLY NEEDS EXTENDED TO 3D WHEN WE START DRAWING THAT
  // SINCE EVERYTHING IS 2D NOW, THIS WOULD BE A USELESS CHECK FOR THE Z PLANE
  // Since, if any the Manhattan distance components between us are greater than the combined radiii
  // of these two particles, there is no way the magnitude of the distance vector could be smaller.
  // This prevents us from having to compute the expensive actual distance, which involves squaring 3x,
  // summing the 3 squares, and taking their root.
  // This early bail-out for example when using the FixedPoint type took the median run time of the run() loop
  // and reduced it from 14.436ms to 2.211, just over a 6.5x improvement.
  if (abs(a.get_position().one() - b.get_position().one()) > min_dist or
      abs(a.get_position().two() - b.get_position().two()) > min_dist) {
    return Status::None;
  }

  const auto dist = a.get_position() - b.get_position();

  // nope too far away
  if (dist.magnitude > min_dist) {
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
  const auto dot_product_abs = abs(impulse_unit_vector ^ v_delta_before);
  const auto impulse_vector = 2 * impulse_unit_vector * (dot_product_abs / (a.get_inverse_mass() + b.get_inverse_mass()));

  DEBUG_MSG(COLLISION_DETECTED);

  // now that we have our impulse vector accounted for, we can calculate post-collision velocities
  a.set_velocity(a.get_velocity() + impulse_vector / a.get_mass());
  b.set_velocity(b.get_velocity() - impulse_vector / b.get_mass());

  // KE is not always conserved in our system.
  // This is likely due to a number of factors I haven't run down yet including
  //   * Insufficient resolution in the system
  //   * Losses in precision due to intermediate values of floats
  //   * Inability to represent irraitonal numbers fully
  // The correction algorithm is able to correct roughly half of erroneous collisions in
  // the random case, but otherwise we simply bail and restore the original velocities.
  const auto ka_after = a.get_kinetic_energy();
  const auto kb_after = b.get_kinetic_energy();
  const auto k_delta = abs((ka_before + kb_before) - (ka_after + kb_after));

  // energy must be conserved
  // for my own sanity, for now, I allow a small delta in collisions. With any form of number representation I have had
  // issues with inexact results yielding slight errors here. Entropy will build in the system but perhaps at an acceptable
  // rate if this number is sufficiently low.
  if (k_delta > 0.1) {
    if (retry) {
      DEBUG_MSG(IMPOSSIBLE_COLLISION_WARNING);

      in_replay_mode = true;
      Status s = correct_collision(a, b);
      in_replay_mode = false;

      // We weren't able to correct these particles... restore their original state
      // and allow them to clip without a collision.
      if (s != Status::Corrected) {
        a.set_velocity(va_before);
        b.set_velocity(vb_before);

        DEBUG_MSG(UNABLE_TO_CORRECT_COLLISION);
        DEBUG_MSG(POST_COLLISION_REPORT);
        return Status::Inconsistent;
      }
      DEBUG_MSG(POST_COLLISION_REPORT);

      return Status::Corrected;
    }
    DEBUG_MSG(IMPOSSIBLE_COLLISION_WARNING);
    DEBUG_MSG(POST_COLLISION_REPORT);

    return Status::Inconsistent;
  }

  DEBUG_MSG(POST_COLLISION_REPORT);

  // that's it!
  return Status::Success;
}

template<typename V>
Status PhysicsContext<V>::correct_collision(Component::Particle<V>& a, Component::Particle<V>& b) {
  // two particles, gone astray. let's see if we can put them on the right course with some mandatory re-education
  const auto& last_state = m_outer_sim->get_particles();

  // get the problem particles in their good days
  const auto& a_old = last_state[a.uid.get() - 1];
  const auto& b_old = last_state[b.uid.get() - 1];

  DEBUG_MSG(COLLISION_CORRECTION_REPORT);

  // This gets a lot more expensive to fix as we go to finer resolutions
  // We'll try half-speed, then quarter-speed, then tenth-speed, then hundreth speed
  std::array<std::tuple<chrono::microseconds, uint8_t>, 4> scales = {
    std::make_tuple(chrono::microseconds(SIM_RESOLUTION_US.count() / 2), 2),
    std::make_tuple(chrono::microseconds(SIM_RESOLUTION_US.count() / 4), 4),
    std::make_tuple(chrono::microseconds(SIM_RESOLUTION_US.count() / 10), 10),
    std::make_tuple(chrono::microseconds(SIM_RESOLUTION_US.count() / 100), 100)
  };

  size_t attempt = 0;
  for (auto scale : scales) {
    attempt++;
    auto a_working = a_old;
    auto b_working = b_old;

    DEBUG_MSG(CORRECTION_ATTEMPT_REPORT);

    // we know a collision will occur at some point soon...
    size_t steps = 0;
    Status s;
    do {
      // replay everything that these particles had happen, at the higher resolution
      gravity(a_working, std::get<0>(scale));
      gravity(b_working, std::get<0>(scale));

      step(a_working, std::get<0>(scale));
      step(b_working, std::get<0>(scale));

      s = collide_internal(a_working, b_working, false);

      for (const auto& wall : m_outer_sim->get_boundaries()) {
        bounce(a_working, wall);
        bounce(b_working, wall);
      }

      steps++;
      // Going more steps than our time division factor would actually put us ahead of the current frame.
      // If these particles keep going they may never necessarily collide again.
    } while(s == Status::None and steps < std::get<1>(scale));

    if (s == Status::Inconsistent) {
      // we failed to correct. try a higher resolution or give up
      DEBUG_MSG(CORRECTION_ATTEMPT_FAILED);
      continue;
    } else if (s == Status::Success) {
      // we did it! correct the particles and let the caller know
      a = a_working;
      b = b_working;

      DEBUG_MSG(CORRECTION_ATTEMPT_SUCCESS);
      return Status::Corrected;
    }
  }
  return Status::Failure;
}

template<typename V>
Status PhysicsContext<V>::bounce(Component::Particle<V>& p, const Component::Wall<V>& wall) {

  // are we traveling toward this wall?
  // velocity sign in relevant direction must oppose normal vector of wall
  auto v_relative = p.get_velocity() * wall.normal().absolute();

  // since this is now a 1D vector, its sum is just the relevant portion... check if signs are opposite
  if ((v_relative.sum() < 0) == (wall.normal().sum() < 0)) {
    return Status::None;
  }

  // Ok we're traveling at the wall, but are we close enough for a collision?
  auto distance = abs((p.get_position() * wall.normal().absolute()).sum() - wall.position());

  if (distance <= static_cast<vector_t>(p.get_radius())) {
    p.set_velocity(p.get_velocity() * wall.inverse());

    DEBUG_MSG(BOUNCE_DETECTION);

    return Status::Success;
  }
  return Status::None;
}

template<typename V>
void PhysicsContext<V>::gravity(Component::Particle<V>& p, US_T us) {
  vector_t time_scalar = chrono::duration_cast<chrono::duration<vector_t>>(us).count();
  p.set_velocity(p.get_velocity() + (m_gravity.get() * time_scalar));
}

// TODO FFS fix this
template <>
void PhysicsContext<Component::Vector<Util::FixedPoint>>::gravity(Component::Particle<Component::Vector<Util::FixedPoint>>& p, US_T us) {
  (void)us;
  Util::FixedPoint time_scalar(0.01);
  p.set_velocity(p.get_velocity() + (m_gravity.get() * time_scalar));
}

template<typename V>
void PhysicsContext<V>::step(Component::Particle<V>& p, US_T us) {
  // move the amount we would expect, with our given velocity
  vector_t time_scalar = chrono::duration_cast<chrono::duration<vector_t>>(us).count();
  p.set_position(p.get_position() + (p.get_velocity() * time_scalar));
}

// TODO FFS fix this
template <>
void PhysicsContext<Component::Vector<Util::FixedPoint>>::step(Component::Particle<Component::Vector<Util::FixedPoint>>& p, US_T us) {
  (void)us;
  // move the amount we would expect, with our given velocity
  Util::FixedPoint time_scalar(0.01);
  p.set_position(p.get_position() + (p.get_velocity() * time_scalar));
}

template class PhysicsContext<Component::Vector<float>>;
template class PhysicsContext<Component::Vector<double>>;
template class PhysicsContext<Component::Vector<Util::FixedPoint>>;

} // namespace Physics
