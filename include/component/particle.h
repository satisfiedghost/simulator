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
// Allows us to use ADL to defer to the correct sqrt/pow impls
using std::sqrt;
using std::pow;

// Sphere particle
// Templated on underlying representation.
template<typename V>
class Particle {

typedef typename V::vector_t vector_t;

public:
  // Create with a params
  Particle(vector_t radius, vector_t mass)
           : Particle() {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = pow(mass, static_cast<vector_t>(-1));
             m_kinetic_energy = calculate_kinetic_energy();
           }

  // Default
  Particle()
           : m_velocity(0, 0, 0)
           , m_position(0, 0, 0)
           , m_radius(Simulation::DefaultSettings<vector_t>.radius_min)
           , m_mass(Simulation::DefaultSettings<vector_t>.mass_min)
           , m_inverse_mass(pow(Simulation::DefaultSettings<vector_t>.mass_min, static_cast<vector_t>(-1)))
           , m_kinetic_energy(calculate_kinetic_energy())
           {}

  // Oh look at you, providing all the parameters
  Particle(vector_t radius, vector_t mass,
           vector_t xv, vector_t yv, vector_t zv,
           vector_t xp, vector_t yp, vector_t zp)
           : Particle(xv, yv, zv, xp, yp, zp) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = pow(mass, static_cast<vector_t>(-1));
             m_kinetic_energy = calculate_kinetic_energy();
           }

  Particle(vector_t xv, vector_t yv, vector_t zv,
           vector_t xp, vector_t yp, vector_t zp)
           : m_velocity(xv, yv, zv)
           , m_position(xp, yp, zp)
           , m_radius(Simulation::DefaultSettings<vector_t>.radius_min)
           , m_mass(Simulation::DefaultSettings<vector_t>.mass_min)
           , m_inverse_mass(pow(Simulation::DefaultSettings<vector_t>.mass_min, static_cast<vector_t>(-1)))
           , m_kinetic_energy(calculate_kinetic_energy())
           {}

  // Create a particle with vectors
  Particle(vector_t radius, vector_t mass, const V& v, const V &p)
           : Particle(v, p) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = pow(mass, static_cast<vector_t>(-1.f));
             m_kinetic_energy = calculate_kinetic_energy();
           }

  Particle(const V v, const V &p)
           : m_velocity(v)
           , m_position(p)
           , m_radius(Simulation::DefaultSettings<vector_t>.radius_min)
           , m_mass(Simulation::DefaultSettings<vector_t>.mass_min)
           , m_inverse_mass(pow(Simulation::DefaultSettings<vector_t>.mass_min, static_cast<vector_t>(-1.f)))
           , m_kinetic_energy(calculate_kinetic_energy())
           {}

  Particle(const Particle<V>& other) {
    this->m_velocity = other.m_velocity;
    this->m_position = other.m_position;
    this->m_radius = other.m_radius;
    this->m_mass = other.m_mass;
    this->m_inverse_mass = other.m_inverse_mass;
    this->m_kinetic_energy = other.m_kinetic_energy;
    this->uid = other.uid;
  }

  // get a copy of this particle's position vector
  const V& get_position() const { return m_position; };

  // get a copy of this particle's velocity vector
  const V& get_velocity() const { return m_velocity; };

  vector_t get_radius() const { return m_radius; };

  vector_t get_mass() const { return m_mass; }

  vector_t get_inverse_mass() const { return m_inverse_mass; }

  vector_t get_kinetic_energy() const { return m_kinetic_energy; }

  void set_velocity(const V& v);

  void set_position(const V& p);

  // UID for this particle
  // 0 is an invalid UID
  Util::LatchingValue<size_t> uid;

  template <typename Vv>
  friend std::ostream& operator<<(std::ostream &os, const Particle<Vv> &);

private:
  // velocity
  V m_velocity;
  // 3d position
  V m_position;
  // radius
  vector_t m_radius;
  //mass
  vector_t m_mass;
  // inverse mass, precalculate this for later use
  vector_t m_inverse_mass;
  // kinetic energy
  vector_t m_kinetic_energy;

  vector_t calculate_kinetic_energy() {
    return static_cast<vector_t>(0.5) * m_mass * pow(m_velocity.magnitude, static_cast<vector_t>(2));
  }
};

// Particle == Particle
template<typename V>
bool operator==(const Particle<V>& first, const Particle<V>& second);

// Print a particle
template<typename V>
std::ostream& operator<<(std::ostream& os, const Particle<V>& p);

} //namespace Component

// Define operators without cluttering header
#include "internal/particle.h"
