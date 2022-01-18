#pragma once
#include "roots.h"

#include <cstdlib>

// Use a pre-computed lookup table to take a reasonably good first guess at the root.
double guess_root(const __int128_t r) {
    size_t bottom = 0, top = ranges.size();
    size_t idx = ranges.size() / 2;

    if (r > std::numeric_limits<int64_t>::max()) {
      return (*ranges.end()).avg;
    }

    // Can't reseat this as a reference, but we want read-only memory access instead
    // of making copies...
    const Range* range = &ranges[idx];

    auto in_range = [&]() -> bool {
        return r >= range->min && r <= range->max;
    };

    // binary search for the relevant range...
    while(!in_range()) {
        // go lower
        if (r < range->min) {
            top = idx;
            idx = (top + bottom) / 2;
        } else { // go higher
            bottom = idx;
            idx = (top + bottom) / 2;
        }

        range = &ranges[idx];
    }

    return range->avg;
}