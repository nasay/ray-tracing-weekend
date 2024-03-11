#include "utility.h"
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "camera.h"
#include "material.h"

int main()
{
    hittable_list world;
    auto material_ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto material_lambertian = make_shared<lambertian>(color(0.1, 0.1, 0.1));
    auto material_dielectric = make_shared<dielectric>(1.5);
    auto material_metal = make_shared<metal>(color(0.8, 0.6, 0.2), 0.01);
    auto material_light = make_shared<diffuse_light>(color(4, 4, 4));

    world.add(make_unique<sphere>(point3(0, -1000, 0), 1000, material_metal));
    world.add(make_unique<sphere>(point3(0, 0, 0), 50, material_metal));
    world.add(make_unique<sphere>(point3(0, 2, 0), 2, material_metal));
    world.add(make_unique<sphere>(point3(0.0, 4.0, 3), 0.5, material_light));
    world.add(make_unique<sphere>(point3(0.0, 4.0, -3), 0.5, material_light));
    world.add(make_unique<sphere>(point3(3.0, 1.0, 0), 0.5, material_light));

    camera cam;
    cam.vfov = 90;
    cam.look_from = point3(4, 3, 0);
    cam.look_at = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);
    cam.defocus_angle = 10.0;
    cam.focus_dist = 5;
    cam.num_samples = 100;
    cam.max_depth = 100;
    cam.render(world, {0.01, 0.01, 0.01});
}