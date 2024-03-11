#pragma once
#include <fstream>
#include <iostream>
#include <omp.h>

#include "utility.h"
#include "color.h"
#include "hittable.h"
#include "material.h"

class camera
{
public:
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int num_samples = 100;
    int max_depth = 10;
    double vfov = 90;
    point3 look_from{0, 0, -1};
    point3 look_at{0, 0, 0};
    vec3 vup{0, 1, 0};

    double defocus_angle = 0; // Variation angle of rays through each pixel
    double focus_dist = 10;   // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable &world, const color &background)
    {
        initialize();
        std::ofstream of("output.ppm");

        of << "P3\n"
           << image_width << " " << image_height << "\n255\n";
        std::vector<std::vector<color>> image(image_height);
        int done = 0;
#pragma omp parallel for schedule(dynamic, 1)
        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - done) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color{};
                for (int s = 0; s < num_samples; s++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, background, world, max_depth);
                }
                image[j].push_back(pixel_color);
            }
#pragma omp atomic
            done++;
        }

        for (int j = 0; j < image_height; ++j)
            for (int i = 0; i < image_width; ++i)
                write_color(of, image[j][i], num_samples);

        std::clog << "\rDone.                 \n";
    }

private:
    int image_height;   // Rendered image height
    point3 pixel00_loc; // Location of pixel 0, 0
    vec3 pixel_delta_u; // Offset to pixel to the right
    vec3 pixel_delta_v; // Offset to pixel below
    point3 center{};
    vec3 u, v, w;
    vec3 defocus_disk_u, defocus_disk_v;

    void initialize()
    {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = [=]()
        {
            int height = image_width / aspect_ratio;
            return (height < 1) ? 1 : height;
        }();
        center = look_from;
        double focal_length = (look_from - look_at).length();
        double theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        double viewport_height = 2 * h * focal_length;
        double viewport_width = viewport_height * double(image_width) / image_height;

        // Calculate u, v, w unit basis vectors for the camera coordinate frame.

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Defocus radius
        double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * (-v);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - focus_dist * w - viewport_u * 0.5 - viewport_v * 0.5;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray &r, const color &background, const hittable &world, int depth) const
    {
        if (depth <= 0)
            return {0, 0, 0};

        hit_record rec;
        if (world.hit(r, {0.001, +infinity}, rec))
        {
            ray scattered;
            color attentuation;
            color emitted = rec.mat->emit();
            if (rec.mat->scatter(r, rec, attentuation, scattered))
                return emitted + attentuation * ray_color(scattered, background, world, depth - 1);

            return emitted;
        }

        return background;
    }

    ray get_ray(int i, int j)
    {
        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_from_o_square();
        auto ray_direction = pixel_sample - center;
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        return {center, ray_direction};
    }

    point3 pixel_sample_from_o_square()
    {
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u + py * pixel_delta_v);
    }

    point3 defocus_disk_sample()
    {
        point3 p = random_in_unit_disk();
        return center + p[0] * defocus_disk_u + p[1] * defocus_disk_v;
    }
};