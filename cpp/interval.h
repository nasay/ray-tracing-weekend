#pragma once

#include "utility.h"

struct interval
{
    double min{+infinity}, max{-infinity};
    interval() = default;
    interval(double min, double max) : min(min), max(max) {}
    bool contains(double x) const
    {
        return x >= min && x <= max;
    }
    bool surrounds(double x) const
    {
        return x > min && x < max;
    }
    double clamp(double x) const
    {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }
    static const interval empty, universe;
};

const static interval empty{+infinity, -infinity};
const static interval universe{-infinity, infinity};