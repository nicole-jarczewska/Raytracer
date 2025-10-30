#include "vec.h"
#pragma once

class ray {
  public:
    ray() {}

    ray(const point& origin, const vec& direction) : orig(origin), dir(direction) {}

    const point& origin() const  { return orig; }
    const vec& direction() const { return dir; }

    point at(double t) const {
        return orig + t*dir;
    }

  private:
    point orig;
    vec dir;
};
