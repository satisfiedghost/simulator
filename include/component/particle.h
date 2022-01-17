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

// If e.g. we bounce, no need to invalidate the
// kinetic energy since it's entirely retained within the particle
enum class EnergyInvalidationPolicy {
  INVALIDATE,
  KEEP
};

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
             m_inverse_mass = 0;
             is_inverse_mass_valid = false;
             m_kinetic_energy = 0;
             is_kinetic_energy_valid = false;
           }

  // Default
  Particle()
           : m_velocity(0, 0, 0)
           , m_position(0, 0, 0)
           , m_radius(Simulation::DefaultSettings<vector_t>.radius_min)
           , m_mass(Simulation::DefaultSettings<vector_t>.mass_min)
           , m_inverse_mass(0)
           , is_inverse_mass_valid(false)
           , m_kinetic_energy(0)
           , is_kinetic_energy_valid(false)
           {}

  // Oh look at you, providing all the parameters
  Particle(vector_t radius, vector_t mass,
           vector_t xv, vector_t yv, vector_t zv,
           vector_t xp, vector_t yp, vector_t zp)
           : Particle(xv, yv, zv, xp, yp, zp) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = 0;
             is_inverse_mass_valid = false;
             m_kinetic_energy = 0;
             is_kinetic_energy_valid = false;
           }

  Particle(vector_t xv, vector_t yv, vector_t zv,
           vector_t xp, vector_t yp, vector_t zp)
           : m_velocity(xv, yv, zv)
           , m_position(xp, yp, zp)
           , m_radius(Simulation::DefaultSettings<vector_t>.radius_min)
           , m_mass(Simulation::DefaultSettings<vector_t>.mass_min)
           , m_inverse_mass(0)
           , is_inverse_mass_valid(false)
           , m_kinetic_energy(0)
           , is_kinetic_energy_valid(false)
           {}

  // Create a particle with vectors
  Particle(vector_t radius, vector_t mass, const V& v, const V &p)
           : Particle(v, p) {
             m_radius = radius;
             m_mass = mass;
             m_inverse_mass = 0;
             is_inverse_mass_valid = false;
             m_kinetic_energy = 0;
             is_kinetic_energy_valid = false;
           }

  Particle(const V v, const V &p)
           : m_velocity(v)
           , m_position(p)
           , m_radius(Simulation::DefaultSettings<vector_t>.radius_min)
           , m_mass(Simulation::DefaultSettings<vector_t>.mass_min)
           , m_inverse_mass(0)
           , is_inverse_mass_valid(false)
           , m_kinetic_energy(0)
           , is_kinetic_energy_valid(false)
           {}

  Particle(const Particle<V>& other) {
    this->m_velocity = other.m_velocity;
    this->m_position = other.m_position;
    this->m_radius = other.m_radius;
    this->m_mass = other.m_mass;
    this->m_inverse_mass = other.m_inverse_mass;
    this->is_inverse_mass_valid = other.is_inverse_mass_valid;
    this->m_kinetic_energy = other.m_kinetic_energy;
    this->is_kinetic_energy_valid = other.is_kinetic_energy_valid;
    this->uid = other.uid;
  }

  // get a copy of this particle's position vector
  const V& position() const { return m_position; };

  // get a copy of this particle's velocity vector
  const V& velocity() const { return m_velocity; };

  const vector_t& radius() const { return m_radius; };

  const vector_t& mass() const { return m_mass; }

  const vector_t& inverse_mass();

  const vector_t& kinetic_energy();

  void set_velocity(const V&, EnergyInvalidationPolicy = EnergyInvalidationPolicy::INVALIDATE);

  void set_position(const V&);

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
  // is it valid?
  bool is_inverse_mass_valid;
  // kinetic energy
  vector_t m_kinetic_energy;
  // is it valid?
  bool is_kinetic_energy_valid;
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
