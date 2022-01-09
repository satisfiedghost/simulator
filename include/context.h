#pragma once
// Include this file for access to *Contexts

// Forward declare so these classes can know about each other
namespace Simulation{
template<typename V>
class PhysicsContext;
}

namespace Simulation{
template<typename V>
class SimulationContext;
}

#include "context/physics.h"
#include "context/simulation.h"