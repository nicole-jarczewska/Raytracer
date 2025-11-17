#pragma once
#include "vec.h"
#include "ray.h"

using color = vec;

color ray_color(const ray& r, int depth, const hittable& world, color background) {
    if (depth <= 0)
            return color(0,0,0);

    hit_record rec;
    // if (world.hit(r, interval(0.001, infinity), rec)) {
    //     ray scattered;
    //     color attenuation;
    //     if (rec.mat->scatter(r, rec, attenuation, scattered))
    //         return attenuation * ray_color(scattered, depth-1, world);
    //     return color(0,0,0);
    // }

    // vec unit_direction = unit_vector(r.direction());
    // auto a = 0.5*(unit_direction.y() + 1.0);
    // return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth-1, world, background);

        return color_from_emission + color_from_scatter;
}


