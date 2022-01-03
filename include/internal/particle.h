#pragma once
#include "latch.h"
#include "sim_settings.h"
#include "sim_time.h"
#include "status.h"
#include "vector.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace Component {

using std::size_t;

template<typename T>
T calculate_kinetic_energy(T mass, Component::Vector<T> velocity) {
  return 0.5f * mass * ::powf(velocity.magnitude, 2);
}

// Point particle
template<typename T>
class Particle {

static_assert(!std::is_integral<T>::value, "Integral types are not yet supported.");
public:
  // Create with a params
  Particle(size_t radius, float mass)
           : Particle() {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = ::powf(mass, -1);
             m_kinetic_energy = calculate_kinetic_energy(m_mass, m_velocity);
           }

  // Default
  Particle()
           : m_velocity(0, 0, 0)
           , m_position(0, 0, 0)
           , m_radius(Simulation::DefaultSettings.radius_min)
           , m_mass(Simulation::DefaultSettings.mass_min)
           , m_inverse_mass(::powf(Simulation::DefaultSettings.mass_min, -1))
           , m_kinetic_energy(calculate_kinetic_energy(m_mass, m_velocity))
           {}

  // Oh look at you, providing all the parameters
  Particle(T xv, T yv, T zv,
           T xp, T yp, T zp,
           size_t radius, float mass)
           : Particle(xv, yv, zv, xp, yp, zp) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = powf(mass, -1);
             m_kinetic_energy = calculate_kinetic_energy(m_mass, m_velocity);
           }

  Particle(T xv, T yv, T zv,
           T xp, T yp, T zp)
           : m_velocity(xv, yv, zv)
           , m_position(xp, yp, zp)
           , m_radius(Simulation::DefaultSettings.radius_min)
           , m_mass(Simulation::DefaultSettings.mass_min)
           , m_inverse_mass(::powf(Simulation::DefaultSettings.mass_min, -1))
           , m_kinetic_energy(calculate_kinetic_energy(m_mass, m_velocity))
           {}

  // Create a particle with vectors
  Particle(size_t radius, float mass, const Vector<T>& v, const Vector<T> &p)
           : Particle(v, p) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = ::powf(mass, -1);
             m_kinetic_energy = calculate_kinetic_energy(m_mass, m_velocity);
           }

  Particle(const Vector<T>& v, const Vector<T> &p)
           : m_velocity(v)
           , m_position(p)
           , m_radius(Simulation::DefaultSettings.radius_min)
           , m_mass(Simulation::DefaultSettings.mass_min)
           , m_inverse_mass(::powf(Simulation::DefaultSettings.mass_min, -1))
           , m_kinetic_energy(calculate_kinetic_energy(m_mass, m_velocity))
           {}

  // moves the particle a total of some time
  // or supply a value to run the simulation forward or backward
  void step(US_T us);

  // get a copy of this particle's position vector
  const Vector<T>& get_position() const { return m_position; };

  // get a copy of this particle's velocity vector
  const Vector<T>& get_velocity() const { return m_velocity; };

  size_t get_radius() const { return m_radius; };

  float get_mass() const { return m_mass; }

  float get_inverse_mass() const { return m_inverse_mass; }

  float get_kinetic_energy() const { return m_kinetic_energy; }

  void set_velocity(const Vector<T>& v);

  // UID for this particle
  // 0 is an invalid UID
  LatchingValue<size_t> uid;

  template <typename S>
  friend std::ostream& operator<<(std::ostream &os, const Particle<S> &t);
private:
  // velocity
  Vector<T> m_velocity;
  // 3d position
  Vector<T> m_position;
  // radius
  size_t m_radius;
  //mass
  float m_mass;
  // inverse mass, precalculate this for later use
  float m_inverse_mass;
  // kinetic energy
  float m_kinetic_energy;
};

template<typename T>
bool operator==(const Particle<T>&, const Particle<T>&);

} //namespace Simulation

#include "particle.tpp"

