#pragma once
#include "hit.h"
#include "texture.h"

class hit_record; 

class material {
  public:
    virtual ~material()=default;

     virtual color emitted(double u, double v, const point& p) const {
        return color(0,0,0);
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const{
      return false;
    }
};

class lambertian : public material {
  public:
    //lambertian(const color& albedo) : albedo(albedo) {}
    lambertian(const color& albedo) : tex(std::make_shared<solid_color>(albedo)) {}
    lambertian(std::shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override{
        auto scatter_direction = rec.normal + random_unit_vector();

        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
      
        scattered = ray(rec.p, scatter_direction);
        //attenuation = albedo;
        attenuation = tex->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    color albedo;
    std::shared_ptr<texture> tex;
};

class metal : public material {
  public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override{
        vec reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec unit_direction = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        // vec refracted = refract(unit_direction, rec.normal, ri);

        // scattered = ray(rec.p, refracted);
        return true;
    }

  private:
    double refraction_index;
    
    static double reflectance(double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};

class glass : public material {
public:
    glass(double index_of_refraction,
          const color& tint_color = color(1.0, 1.0, 1.0))
        : ior(index_of_refraction),
          tint(tint_color),
          inner_dielectric(std::make_shared<dielectric>(index_of_refraction))
    {}

    virtual bool scatter(const ray& r_in, const hit_record& rec,
                         color& attenuation, ray& scattered) const override 
    {
        bool scattered_ok = inner_dielectric->scatter(r_in, rec, attenuation, scattered);

        attenuation *= tint;

        return scattered_ok;
    }

private:
    double ior;
    color tint;
    std::shared_ptr<material> inner_dielectric;
};

class diffuse_light : public material {
  public:
    diffuse_light(std::shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const color& emit) : tex(std::make_shared<solid_color>(emit)) {}

    color emitted(double u, double v, const point& p) const override {
        return tex->value(u, v, p);
    }

  private:
    std::shared_ptr<texture> tex;
};
