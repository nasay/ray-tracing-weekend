#pragma once

#include "utility.h"
#include "hittable.h"

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray &, const hit_record &, color &, ray &) const = 0;
    virtual color emit() const
    {
        return {0, 0, 0};
    }
};

class lambertian : public material
{
public:
    lambertian(const color &a) : albedo(a) {}
    bool scatter(const ray &r, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        vec3 scattered_direction = rec.normal + random_unit_vector();
        if (scattered_direction.is_near_zero())
            scattered_direction = rec.normal;
        scattered = {rec.p, std::move(scattered_direction)};
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

class metal : public material
{
public:
    metal(const color &a, double f) : albedo(a), fuzziness(f) {}
    bool scatter(const ray &r, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        vec3 scattered_direction = reflect(unit_vector(r.direction()), rec.normal) + fuzziness * random_unit_vector();
        scattered = {rec.p, std::move(scattered_direction)};
        attenuation = albedo;
        return (dot(scattered_direction, rec.normal) > 0);
    }

private:
    color albedo;
    double fuzziness;
};

class dielectric : public material
{
public:
    dielectric(double ir) : refraction_index(ir){};
    bool scatter(const ray &r, const hit_record &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = {1.0, 1.0, 1.0};
        double refraction_ratio = rec.front_face ? 1.0 / refraction_index : refraction_index;
        vec3 unit_direction = unit_vector(r.direction());
        double cos_theta = fmin(dot(unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1 - cos_theta * cos_theta);
        vec3 direction;
        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        {
            direction = reflect(unit_direction, rec.normal);
        }
        else
        {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }
        scattered = {rec.p, direction};

        return true;
    }

private:
    double refraction_index;
    static double reflectance(double cosine, double refraction_ratio)
    {
        // Schlick's approximation.
        auto r0 = (1 - refraction_ratio) / (1 + refraction_ratio);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow(1 - cosine, 5);
    }
};

class diffuse_light : public material
{
public:
    diffuse_light(color c) : c(c) {}
    bool scatter(const ray &, const hit_record &, color &, ray &) const override
    {
        return false;
    }
    virtual color emit() const override
    {
        return c;
    }

private:
    color c;
};