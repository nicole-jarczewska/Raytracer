#pragma once
#include "hit.h"
#include "material.h"


class sphere : public hittable {
public:
    sphere(const point& center, double radius, std::shared_ptr<material> mat);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    void get_sphere_uv(const point& p, double& u, double& v) const;

private:
    point center;
    double radius;
    std::shared_ptr<material> mat;

};

class cylinder : public hittable {
public:
    cylinder(point center,
             double radius,
             double height,
             std::shared_ptr<material> mat,
             bool capped = true);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    void get_cap_uv(const point& p, double& u, double& v, bool top) const;

private:
    point cen;
    double rad;
    double h;
    bool caps;
    std::shared_ptr<material> mat;

    bool check_cylinder_root(const ray& r, double t, double t_min, double t_max, hit_record& rec) const;
    bool intersect_cap(const ray& r, double y_offset, double t_min, double t_max, hit_record& rec) const;
};

class cone : public hittable {
public:
    cone(point tip,
         double radius,
         double height,
         std::shared_ptr<material> mat,
         bool capped = true);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    point tip;
    double rad;
    double h;
    bool caps;
    std::shared_ptr<material> mat;

    bool check_root(const ray& r, double t, double t_min, double t_max, hit_record& rec) const;
};


class shape : public hittable {
public:
    shape() = default;
    shape(point center,
             double radius_bottom,
             double radius_top,
             double height,
             std::shared_ptr<material> mat,
             bool capped);

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    point cen;
    double rad_bottom;
    double rad_top;
    double h;
    bool caps;
    std::shared_ptr<material> mat;

    bool intersect_cap(const ray& r, double y_offset, double radius_at_y,
                       double t_min, double t_max, hit_record& rec) const;
};

struct tri_vertex {
    point p;
    vec normal;
};

struct tri_face {
    tri_vertex v0, v1, v2;
    std::shared_ptr<material> mat;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const;
};

class rock_floor : public hittable {
public:
    rock_floor(const point& center,
               double size_x,
               double size_z,
               int grid_res_x,
               int grid_res_z,
               std::shared_ptr<material> mat);

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    std::vector<tri_face> faces;
};
