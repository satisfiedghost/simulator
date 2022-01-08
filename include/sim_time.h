#pragma once
#include <chrono>

namespace chrono = std::chrono;

using US_T = chrono::microseconds;
using MS_T = chrono::milliseconds;
using S_T =  chrono::seconds;

constexpr size_t US_IN_S = 1'000'000UL;

// Time resolution of the physics engine, this is guaranteed to be the most (approximately) real
// time between updates. Decrease to tradeoff fidelity for performance.
static constexpr chrono::microseconds SIM_RESOLUTION_US{10'000UL};

// How many discrete engine ticks we get every second.
static constexpr size_t TICKS_PER_SECOND = US_IN_S / SIM_RESOLUTION_US.count();