#pragma once
#include "roots.h"

#include <cstdlib>

struct Range {
int min;
int max;
double avg;
};

double locate_root(const int r) {
    size_t bottom = 0, top = ranges.size();
    size_t idx = ranges.size() / 2;


    auto& range = ranges[idx];

    auto in_range = [&]() -> bool {
        return r >= range.min && r <= range.max;
    };

    while(!in_range()) {
        // go lower
        if (r < range.min) {
            top = idx;
            idx = (top + bottom) / 2;
        } else { // go higher
            bottom = idx;
            idx = (top + bottom) / 2;
        }

        range = ranges[idx];
    }

    return range.avg;
}