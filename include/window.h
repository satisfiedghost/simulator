#pragma once
#include "simulation.h"

#include <tuple>
#include <vector>

namespace Graphics {

void SimulationWindowThread(const Simulation::SimulationContext& sim, Simulation::SimSettings settings);

// Get the dimensions of the screen.
// TODO: We aren't drawing Z yet so it just returns the default value....
std::tuple<size_t, size_t, size_t> get_window_size();

}
