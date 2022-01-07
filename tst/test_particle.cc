#include "component.h"
#include "context.h"
#include "sim_settings.h"

#include <array>
#include <gtest/gtest.h>
#include <math.h>

using namespace Simulation;
using Component::Vector;
using Component::Particle;

class ParticleTest :
  public ::testing::Test {};

constexpr size_t PARTICLE_RADIUS = 20;

// test our assumptions about Particle Collisions
TEST_F(ParticleTest, Collisions2D) {
  Simulation::PhysicsContext<float> phys;

  constexpr size_t N_ANGLES = 360;
  constexpr float DISTANCE = 2.f * static_cast<float>(PARTICLE_RADIUS) - 0.05f; // a reasonable trigger for the simulation

  // unfortunately this test would take just shy of 2 years to run at the fidelity I'd like
  constexpr size_t CHECK_RADS = 100;
  constexpr size_t CHECK_VELOCITIES = 10;
  constexpr float MAX_VELOCITY = 5;

  constexpr float velocity_step = MAX_VELOCITY * 2.f / static_cast<float>(CHECK_VELOCITIES); // TODO: would this be better randomized?
  constexpr float radian_step = 2.f * M_PI / static_cast<float>(CHECK_RADS);

  std::array<float, CHECK_VELOCITIES> velocities;
  std::array<float, CHECK_RADS> milli_rads;

  for (size_t i = 0; i < CHECK_VELOCITIES; i++) {
    velocities[i] = i * velocity_step;
  }

  for (size_t i = 0; i < CHECK_RADS; i++) {
    milli_rads[i] = i * radian_step;
  }

  size_t scenario_count = milli_rads.size() * velocities.size() * velocities.size()
                          * milli_rads.size() * milli_rads.size();

  std::cout << "Running through " << scenario_count << " test cases." << std::endl;

  auto deg_to_rad = [](float angle) {
    return angle * M_PI / 180;
  };

  auto rad_to_degree = [](float angle) {
    return angle * 180 / M_PI;
  };

  size_t scenarios_run = 0;
  size_t outer_loop_cnt = 0;

  // Put ball A at origin and rotate B around it
  for (auto rad : milli_rads) {
    Vector<float> a_pos(0, 0, 0);
    auto b_x_pos = DISTANCE * cos(rad * 1000.f);
    auto b_y_pos = DISTANCE * sin(rad * 1000.f);
    Vector<float> b_pos(b_x_pos, b_y_pos, 0);

    // run each through its respective velocity...
    for (auto v_a : velocities) {
      // oh
      for (auto v_b : velocities) {
        // god
        for (auto v_a_rad : milli_rads) {
          // help
          for (auto v_b_rad : milli_rads) {
            // CPU smoke break
            auto a_x_vel = v_a * cos(v_a_rad * 1000.f);
            auto a_y_vel = v_a * sin(v_a_rad * 1000.f);
            auto b_x_vel = v_b * cos(v_b_rad * 1000.f);
            auto b_y_vel = v_b * sin(v_b_rad * 1000.f);
            Vector<float> a_velocity(a_x_vel, a_y_vel, 0);
            Vector<float> b_velocity(b_x_vel, b_y_vel, 0);

            // wow finally create some particles
            Particle<float> a(a_velocity, a_pos);
            Particle<float> b(b_velocity, b_pos);

            auto energy_a_before = std::pow(a.get_velocity().magnitude, 2);
            auto energy_b_before = std::pow(b.get_velocity().magnitude, 2);
            auto total_energy_before = energy_a_before + energy_b_before;

            phys.collide(a, b);
            auto energy_a_after = std::pow(a.get_velocity().magnitude, 2);
            auto energy_b_after = std::pow(b.get_velocity().magnitude, 2);
            auto total_energy_after = energy_a_after + energy_b_after;

            auto delta = total_energy_after - total_energy_before;
            scenarios_run++;
            // energy MUST be conserved!
            ASSERT_EQ(delta, 0)
            << "Energy was not conserved!" << std::endl
            << "The energy before was " << total_energy_before << " and after was " << total_energy_after << std::endl
            << "This is a delta of " << delta << std::endl << "Here's some collision metadata: " << std::endl
            << "Va before: " << std::endl << a_velocity << std::endl << "Vb before: " << std::endl << b_velocity << std::endl
            << "Va after: " << std::endl << a.get_velocity() << std::endl << "Vb after: " << std::endl << b.get_velocity() << std::endl
            << "You might want to sketch out what this collision looked like, remember A is always centered at the origin" << std::endl 
            << "B was centered at: " << std::endl << b_pos << std::endl
            << "Whichs puts the A->B vector at an angle of " << rad_to_degree(rad) << std::endl
            << "A's velocity vector was at an angle of " << rad_to_degree(v_a_rad) << std::endl
            << "B's velocity vecotor was at an angle of " << rad_to_degree(v_b_rad) << std::endl
            << "This was scenario # " << scenarios_run << std::endl;
          }
        }
      }
    }
    outer_loop_cnt++;
    std::cout << "We've run " << scenarios_run << " scenarios and are "
    << static_cast<float>(outer_loop_cnt) / static_cast<float>(milli_rads.size()) * 100 << "% done" << std::endl;
  }
}