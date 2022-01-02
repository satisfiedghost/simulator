#pragma once
#include "latch.h"
#include <vector>

namespace Simulation {

enum class ScreenMode {
  FULLSCREEN,
  DEFAULT,
};

// global settings that are held to be invariant across the system
// generally we should move to allow these to be reconfigured or variable across the system
// but you've gotta start somewhere
struct SimSettings {
  size_t particle_radius;       ///<< radius of all particles
  size_t number_particles;      ///<< total particles in the system
  int vmax;                     ///<< magnitude of maximum starting velocity of all particles
  int vmin;                     ///<< magnitude of maximum starting velocity of all particles
  float angle;                  ///<< starting angle for velocity of all particles (TODO this is only 2D applicable right now)
  bool random_angle;            ///<< whether particles should have a random direction of travel to start. setting angle overrides this
  std::vector<int> color;       ///<< the color of all particles leave unspecified for random colors
  std::vector<int> color_range; ///<< if specified, colors start from color and have an RGB gradient to this color
  bool display_mode;            ///<< don't run the simulation, just display starting conditions
  float delay;                  ///<< display starting conditions for this long before running
  size_t x_width;               ///<< width of system boundaries
  size_t y_width;               ///<< height of system boundaries
  size_t z_width;               ///<< depth of system boundaries
  ScreenMode screen_mode;        ///<< open in full screen or whatever the system default is
};

// Copy this object to get some default settings.
// I really wish C++ < C++20 supported desginated initializers...
const SimSettings DefaultSettings{
  /* .particle_radius */  10,
  /* .number_particles */ 400,
  /* .vmax */             100,
  /* .vmin */             0,
  /* .angle */            0,
  /* .random_angle */     true,
  /* .color */            std::vector<int>(),
  /* .color_range */      std::vector<int>(),
  /* .display_mode */     false,
  /* .delay */            0,
  /* .x_width */          1000,
  /* .y_width */          1000,
  /* .z_width */          1000,
  /* .screen_mode */      ScreenMode::FULLSCREEN
};

} // namespace Simulation