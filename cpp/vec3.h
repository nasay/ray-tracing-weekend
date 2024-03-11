#pragma once

#include <cmath>
#include <iostream>

class vec3
{
public:
    double e[3]{};
    vec3() = default;
    vec3(double x, double y, double z) : e{x, y, z} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return {-e[0], -e[1], -e[2]}; }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    vec3 &operator+=(const vec3 &other)
    {
        e[0] += other.e[0];
        e[1] += other.e[1];
        e[2] += other.e[2];
        return *this;
    }

    vec3 &operator*=(double k)
    {
        e[0] *= k;
        e[1] *= k;
        e[2] *= k;
        return *this;
    }

    vec3 &operator/=(double k)
    {
        return *this *= 1 / k;
    }

    double length_sqr()
    {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    double length()
    {
        return sqrt(length_sqr());
    }

    bool is_near_zero()
    {
        constexpr auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }

    static vec3 random()
    {
        return {random_double(), random_double(), random_double()};
    }

    static vec3 random(double min, double max)
    {
        return {random_double(min, max), random_double(min, max), random_double(min, max)};
    }
};

using point3 = vec3;

inline std::ostream &operator<<(std::ostream &os, const vec3 &v)
{
    return os << v.e[0] << " " << v.e[1] << " " << v.e[2];
}

inline vec3 operator+(const vec3 &a, const vec3 &b)
{
    return {a.e[0] + b.e[0], a.e[1] + b.e[1], a.e[2] + b.e[2]};
}

inline vec3 operator-(const vec3 &a, const vec3 &b)
{
    return {a.e[0] - b.e[0], a.e[1] - b.e[1], a.e[2] - b.e[2]};
}

inline vec3 operator*(const vec3 &a, const vec3 &b)
{
    return vec3(a.e[0] * b.e[0], a.e[1] * b.e[1], a.e[2] * b.e[2]);
}

inline vec3 operator*(const vec3 &a, double k)
{
    return {a.e[0] * k, a.e[1] * k, a.e[2] * k};
}

inline vec3 operator*(double k, const vec3 &a)
{
    return a * k;
}

inline vec3 operator/(const vec3 &a, double k)
{
    return a * (1 / k);
}

inline double dot(const vec3 &a, const vec3 &b)
{
    return a.e[0] * b.e[0] + a.e[1] * b.e[1] + a.e[2] * b.e[2];
}

inline vec3 cross(const vec3 &a, const vec3 &b)
{
    return {a.e[1] * b.e[2] - a.e[2] * b.e[1],
            a.e[2] * b.e[0] - a.e[0] * b.e[2],
            a.e[0] * b.e[1] - a.e[1] * b.e[0]};
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}

inline vec3 random_in_unit_disk()
{
    while (true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_sqr() < 1)
            return p;
    }
}

inline vec3 random_in_unit_sphere()
{
    while (true)
    {
        vec3 v = vec3::random();
        if (v.length_sqr() < 1)
            return v;
    }
}

inline vec3 random_unit_vector()
{
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_unit_on_hemisphare(const vec3 &n)
{
    vec3 v = random_unit_vector();
    return (dot(v, n) > 0.0) ? v : -v;
}

vec3 reflect(const vec3 &v, const vec3 &n)
{
    return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat)
{
    double cos_theta = fmin(dot(unit_vector(-uv), n), 1.0);
    vec3 r_per = etai_over_etat * (uv + cos_theta * n);
    vec3 r_par = -sqrt(fabs(1.0 - r_per.length_sqr())) * n;
    return r_per + r_par;
}