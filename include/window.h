#pragma once
#include "simulation.h"

#include <tuple>
#include <vector>

namespace Graphics {

template <typename T>
void SimulationWindowThread(const Simulation::SimulationContext<T>& sim, Simulation::SimSettings settings);

// Get the dimensions of the screen.
std::tuple<size_t, size_t, size_t> get_window_size();

}
