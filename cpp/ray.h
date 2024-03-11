#pragma once

#include "vec3.h"

class ray
{
public:
    ray() = default;
    ray(point3 origin, vec3 dir) : m_origin(origin), m_dir(dir) {}
    vec3 direction() const { return m_dir; }
    point3 origin() const { return m_origin; }
    point3 at(double t) const
    {
        return m_origin + t * m_dir;
    }

private:
    point3 m_origin{};
    vec3 m_dir{};
};