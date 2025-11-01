#include "vec.h"
#include "ray.h"
#include "objects.h"
#pragma once

using color = vec;

color ray_color(const ray& r, const hittable& world) {
    const double infinity = std::numeric_limits<double>::infinity();
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        vec direction = random_on_hemisphere(rec.normal);
        return 0.5 * ray_color(ray(rec.p, direction), world);
    }

    vec unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

