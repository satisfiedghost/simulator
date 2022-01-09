#pragma once

#include "component.h"
#include "context.h"

#include <random>

namespace Demo {

template<typename V>
void set_initial_conditions(Simulation::SimulationContext<V>& sim, Simulation::SimSettings<typename V::vector_t> settings);

} // Demo