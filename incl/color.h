#include "vec.h"
#include "ray.h"
#pragma once

using color = vec;


//work in progress, tu trzeba dac funkcje ktora liczy kolory ig
color ray_color(const ray& r) {
    vec unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}
