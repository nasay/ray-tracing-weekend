#pragma once

#include <functional>
#include "vec3.h"
#include "ray.h"
#include "interval.h"

class material;

struct hit_record
{
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    shared_ptr<material> mat;
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    hittable() = default;
    virtual ~hittable() {}
    hittable(hittable &) = delete;
    hittable &operator=(hittable &) = delete;
    hittable(hittable &&) = default;
    hittable &operator=(hittable &&) = default;
    virtual bool hit(const ray &r, const interval &, hit_record &rec) const = 0;
};

class sphere : public hittable
{
public:
    using strategy = std::function<bool(const sphere &, const ray &, const interval &, hit_record &)>;
    point3 center;
    double radius;
    sphere(point3 center, double radius, shared_ptr<material> mat)
        : center(center), radius(radius), mat(mat) {}
    bool hit(const ray &r, const interval &range, hit_record &rec) const override
    {
        return hit_strategy(*this, r, range, rec);
    }
    friend bool hit_sphere(const sphere &s, const ray &r, const interval &, hit_record &rec);

private:
    strategy hit_strategy{hit_sphere};
    shared_ptr<material> mat;
};

bool hit_sphere(const sphere &s, const ray &r, const interval &range, hit_record &rec)
{
    vec3 oc = r.origin() - s.center;
    auto a = dot(r.direction(), r.direction());
    auto b = dot(oc, r.direction());
    auto c = dot(oc, oc) - s.radius * s.radius;
    auto discriminant = b * b - a * c;

    if (discriminant < 0)
        return false;

    double root = (-b - sqrt(discriminant)) / a;
    if (!range.surrounds(root))
    {
        root = (-b + sqrt(discriminant)) / a;
        if (!range.surrounds(root))
            return false;
    }

    rec.t = root;
    rec.p = r.at(root);
    rec.set_face_normal(r, (rec.p - s.center) / s.radius);
    rec.mat = s.mat;

    return true;
}