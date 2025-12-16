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

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

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

struct vertex {
    point p;
    vec normal;
    double u, v;
};

// Triangle face with per-vertex UVs
struct tri_face {
    vertex v0, v1, v2;
    std::shared_ptr<material> mat;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {
        const double EPS = 1e-8;
        vec edge1 = v1.p - v0.p;
        vec edge2 = v2.p - v0.p;
        vec h = cross(r.direction(), edge2);
        double a = dot(edge1, h);
        if (std::abs(a) < EPS) return false;

        double f = 1.0 / a;
        vec s = r.origin() - v0.p;
        double u = f * dot(s, h);
        if (u < 0.0 || u > 1.0) return false;

        vec q = cross(s, edge1);
        double v = f * dot(r.direction(), q);
        if (v < 0.0 || u + v > 1.0) return false;

        double t = f * dot(edge2, q);
        if (t < ray_t.min || t > ray_t.max) return false;

        rec.t = t;
        rec.p = r.at(t);

        vec n = cross(edge1, edge2);
        rec.set_face_normal(r, unit_vector(n));
        rec.mat = mat;

        // Interpolate UVs at hit point
        double w = 1.0 - u - v;
        rec.u = w * v0.u + u * v1.u + v * v2.u;
        rec.v = w * v0.v + u * v1.v + v * v2.v;

        return true;
    }
};

// Rock floor class
class rock_floor : public hittable {
public:
    rock_floor(const point& center,
               double size_x,
               double size_z,
               int grid_res_x,
               int grid_res_z,
               std::shared_ptr<material> mat,
               double texture_repeat = 1.0);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    std::vector<tri_face> faces;
};

class rectangle : public hittable {
public:
    rectangle(const point& center, double a, double b, std::shared_ptr<material> mat);
    
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    
private:
    point cen;
    double a;
    double b;
    std::shared_ptr<material> mat;
};

class triangle : public hittable {
public:
    triangle(const point& center, double size, std::shared_ptr<material> mat, bool up);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    std::tuple<point, point, point> tri_points;

private:
    point cen;
    double s;
    std::shared_ptr<material> mat;
    bool up;

    std::tuple<point, point, point> is_up(bool upi) const;

    double h_1 = s / 3;
    double h_2 = h_1 * 2;
    double half = sqrt((h_2 * h_2) - (h_1 * h_1));
};

class snowflake : public hittable {
public:
    snowflake(const point& center, double size, int iteration, std::shared_ptr<material> mat);

    std::vector<triangle> gener() const;

    point halfway(point Q, point W) const;

    std::pair<point, double> where_to_place_down(point Q, point W) const;
    std::pair<point, double> where_to_place_up(point Q, point W) const;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    point cen;
    double s;
    int it;
    std::shared_ptr<material> mat;
    std::vector<triangle>  fig = gener();
};
