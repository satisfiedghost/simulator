#pragma once
#include "context.h"

#include <tuple>
#include <vector>

namespace Graphics {

template <typename V>
void SimulationWindowThread(const Simulation::SimulationContext<V>& sim, Simulation::SimSettings<typename V::vector_t> settings);

// Get the dimensions of the screen.
template<typename VT>
std::tuple<size_t, size_t, size_t> get_window_size();

}
