#pragma once
#include "util/latch.h"

#include <algorithm>
#include <vector>

namespace Simulation {

enum class ScreenMode {
  FULLSCREEN,
  DEFAULT,
};

// global settings that are held to be invariant across the system
// generally we should move to allow these to be reconfigured or variable across the system
// or moved to their respective components to be further individualized
// but you've gotta start somewhere
template<typename VT>
struct SimSettings {
  VT radius_min;                ///<< radius of all particles
  VT radius_max;                ///<< radius of all particles
  VT mass_min;                  ///<< the minimum mass a particle can have
  VT mass_max;                  ///<< the maximum mass a particle can have
  size_t number_particles;      ///<< total particles in the system
  int vmin;                     ///<< magnitude of maximum starting velocity of all particles
  int vmax;                     ///<< magnitude of maximum starting velocity of all particles
  float angle;                  ///<< starting angle for velocity of all particles (TODO this is only 2D applicable right now)
  bool random_angle;            ///<< whether particles should have a random direction of travel to start. setting angle overrides this
  std::vector<int> color;       ///<< the color of all particles leave unspecified for random colors
  std::vector<int> color_range; ///<< if specified, colors start from color and have an RGB gradient to this color
  bool display_mode;            ///<< don't run the simulation, just display starting conditions
  float delay;                  ///<< display starting conditions for this long before running
  size_t x_width;               ///<< width of system boundaries
  size_t y_width;               ///<< height of system boundaries
  size_t z_width;               ///<< depth of system boundaries
  ScreenMode screen_mode;       ///<< open in full screen or whatever the system default is
  float overlap_detection;      ///<< if particles collide and their distance is < this * (R1 + R1) we assume they're overlapping
  bool no_gui;                  ///<< set to run the simulator headless
  std::vector<size_t> trace;    ///<< elide debug information not related to these particles
  bool extra_trace;             ///<< print out state of everyone else in the trace, when any traced event occurs
  VT gravity;                   ///<< I don't think you understand the gravity of the situation
  float gravity_angle;          ///<< I don't think you understand the gravity of the situation
  bool info;                    ///<< Print out INFO level messages.

  static constexpr size_t RingBufferSize = 10;
};

// Copy this object to get some default settings.
// I really wish C++ < C++20 supported desginated initializers...
template <typename T>
const SimSettings<T> DefaultSettings{
  /* .radius_min */             10,
  /* .radius_max */             20,
  /* .mass_min */               1,
  /* .mass_max */               10,
  /* .number_particles */       400,
  /* .vmin */                   0,
  /* .vmax */                   100,
  /* .angle */                  0,
  /* .random_angle */           true,
  /* .color */                  std::vector<int>(),
  /* .color_range */            std::vector<int>(),
  /* .display_mode */           false,
  /* .delay */                  0,
  /* .x_width */                1000,
  /* .y_width */                1000,
  /* .z_width */                1000,
  /* .screen_mode */            ScreenMode::FULLSCREEN,
  /* .overlap_detection */      0.95f,
  /* .no_gui */                 false,
  /* .trace */                  std::vector<size_t>(),
  /* .extra_trace */            false,
  /* .gravity */                0,
  /* .gravity_angle */          270,
  /* .info */                   false
};

inline bool trace_present(const std::vector<size_t>& v, size_t puid) {
  // if no trace was specified, print all. otherwise, check if this is a relevant particle
  return v.size() == 0 or std::find(v.begin(), v.end(), puid) != v.end();
}

} // namespace Simulation