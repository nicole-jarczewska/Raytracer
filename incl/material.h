#pragma once
#include "objects.h"
#include "hit.h"

class hit_record; 

class material {
  public:
    virtual ~material()=default;

    virtual bool scratter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const{
      return false;
    }
};

class lambertian : public material {
  public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const {
        auto scatter_direction = rec.normal + random_unit_vector();
        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};
