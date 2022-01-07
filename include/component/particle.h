#pragma once
#include "util/latch.h"
#include "sim_settings.h"
#include "sim_time.h"
#include "util/status.h"
#include "vector.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace Component {

using std::size_t;

template<typename T>
T calculate_kinetic_energy(T mass, Component::Vector<T> velocity) {
  return static_cast<T>(0.5) * mass * std::pow(velocity.magnitude, 2.f);
}

// Point particle
template<typename T>
class Particle {

static_assert(!std::is_integral<T>::value, "Integral types are not yet supported.");
public:
  // Create with a params
  Particle(size_t radius, T mass)
           : Particle() {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = std::pow(mass, static_cast<T>(-1.f));
             m_kinetic_energy = calculate_kinetic_energy(m_mass, m_velocity);
           }

  // Default
  Particle()
           : m_velocity(0, 0, 0)
           , m_position(0, 0, 0)
           , m_radius(Simulation::DefaultSettings.radius_min)
           , m_mass(Simulation::DefaultSettings.mass_min)
           , m_inverse_mass(std::pow(Simulation::DefaultSettings.mass_min, static_cast<T>(-1.f)))
           , m_kinetic_energy(calculate_kinetic_energy(m_mass, m_velocity))
           {}

  // Oh look at you, providing all the parameters
  Particle(size_t radius, float mass,
           T xv, T yv, T zv,
           T xp, T yp, T zp)
           : Particle(xv, yv, zv, xp, yp, zp) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = std::pow(mass, -1.f);
             m_kinetic_energy = calculate_kinetic_energy(m_mass, m_velocity);
           }

  Particle(T xv, T yv, T zv,
           T xp, T yp, T zp)
           : m_velocity(xv, yv, zv)
           , m_position(xp, yp, zp)
           , m_radius(Simulation::DefaultSettings.radius_min)
           , m_mass(Simulation::DefaultSettings.mass_min)
           , m_inverse_mass(std::pow(Simulation::DefaultSettings.mass_min, static_cast<T>(-1.f)))
           , m_kinetic_energy(calculate_kinetic_energy(m_mass, m_velocity))
           {}

  // Create a particle with vectors
  Particle(size_t radius, T mass, const Vector<T>& v, const Vector<T> &p)
           : Particle(v, p) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = std::pow(mass, static_cast<T>(-1.f));
             m_kinetic_energy = calculate_kinetic_energy(m_mass, m_velocity);
           }

  Particle(const Vector<T>& v, const Vector<T> &p)
           : m_velocity(v)
           , m_position(p)
           , m_radius(Simulation::DefaultSettings.radius_min)
           , m_mass(Simulation::DefaultSettings.mass_min)
           , m_inverse_mass(std::pow(Simulation::DefaultSettings.mass_min, static_cast<T>(-1.f)))
           , m_kinetic_energy(calculate_kinetic_energy(m_mass, m_velocity))
           {}

  Particle(const Particle<T>& other) {
    this->m_velocity = other.m_velocity;
    this->m_position = other.m_position;
    this->m_radius = other.m_radius;
    this->m_mass = other.m_mass;
    this->m_inverse_mass = other.m_inverse_mass;
    this->m_kinetic_energy = other.m_kinetic_energy;
    this->uid = other.uid;
  }

  // get a copy of this particle's position vector
  const Vector<T>& get_position() const { return m_position; };

  // get a copy of this particle's velocity vector
  const Vector<T>& get_velocity() const { return m_velocity; };

  size_t get_radius() const { return m_radius; };

  T get_mass() const { return m_mass; }

  T get_inverse_mass() const { return m_inverse_mass; }

  T get_kinetic_energy() const { return m_kinetic_energy; }

  void set_velocity(const Vector<T>& v);

  void set_position(const Vector<T>& p);

  // UID for this particle
  // 0 is an invalid UID
  Util::LatchingValue<size_t> uid;

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
  T m_mass;
  // inverse mass, precalculate this for later use
  T m_inverse_mass;
  // kinetic energy
  T m_kinetic_energy;
};

// Particle == Particle
template<typename T>
bool operator==(const Particle<T>& first, const Particle<T>& second);

// Print a particle
template<typename T>
std::ostream& operator<<(std::ostream& os, const Particle<T>& p);

} //namespace Component

// Define operators without cluttering header
#include "internal/particle.h"
