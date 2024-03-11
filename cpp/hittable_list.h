#pragma once

#include <vector>
#include <memory>
#include "hittable.h"

class hittable_list : public hittable
{
public:
    hittable_list() = default;
    hittable_list(std::unique_ptr<hittable> &&h)
    {
        add(std::move(h));
    }
    void add(std::unique_ptr<hittable> &&h)
    {
        objects.push_back(std::move(h));
    }
    bool hit(const ray &r, const interval &range, hit_record &rec) const override
    {
        return hit_strategy(*this, r, range, rec);
    }

private:
    using strategy = std::function<bool(const hittable_list &, const ray &, const interval &, hit_record &)>;
    std::vector<std::unique_ptr<hittable>> objects;
    friend bool hit_list(const hittable_list &list, const ray &r, const interval &range, hit_record &rec);
    strategy hit_strategy{hit_list};
};

bool hit_list(const hittable_list &list, const ray &r, const interval &range, hit_record &rec)
{
    bool hit_anything = false;
    auto closest_so_far = range.max;

    for (const auto &obj : list.objects)
    {
        hit_record temp_rec;
        bool hit = obj->hit(r, range, temp_rec);
        if (hit && temp_rec.t < closest_so_far)
        {
            rec = temp_rec;
            closest_so_far = temp_rec.t;
        }
        hit_anything = hit_anything || hit;
    }

    return hit_anything;
}