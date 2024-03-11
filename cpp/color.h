#pragma once

#include "vec3.h"
#include "interval.h"
#include "ostream"

using color = vec3;

static inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

void write_color(std::ostream &os, color color, int sample_per_pixel)
{
    static const interval intensity{0.0, 0.999};
    double scale = 1.0 / sample_per_pixel;
    int r = intensity.clamp(linear_to_gamma(color.x() * scale)) * 256;
    int g = intensity.clamp(linear_to_gamma(color.y() * scale)) * 256;
    int b = intensity.clamp(linear_to_gamma(color.z() * scale)) * 256;
    os << r << " " << g << " " << b << "\n";
}
