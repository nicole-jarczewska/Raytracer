#include "vec.h"
#include "ray.h"
#include "objects.h"
#pragma once

using color = vec;

color ray_color(const ray& r, int depth, const hittable& world) {
    if (depth <= 0)
            return color(0,0,0);

    hit_record rec;
    if (world.hit(r, interval(0.001, infinity), rec)) {
        vec direction = rec.normal + random_on_hemisphere();
        return 0.5 * ray_color(ray(rec.p, direction), depth-1, world);
        //return 0.5 * (rec.normal + color(1,1,1));
    }

    vec unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

