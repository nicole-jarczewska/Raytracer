#include "incl/objects.h"

rock_floor::rock_floor(const point& center,
                       double size_x,
                       double size_z,
                       int grid_res_x,
                       int grid_res_z,
                       std::shared_ptr<material> mat) {
    std::vector<std::vector<double>> heights(grid_res_x, std::vector<double>(grid_res_z, 0.0));

    for (int i = 0; i < grid_res_x; ++i) {
        for (int j = 0; j < grid_res_z; ++j) {
            double u = static_cast<double>(i)/(grid_res_x-1);
            double v = static_cast<double>(j)/(grid_res_z-1);
            heights[i][j] = 0.2*std::sin(3.0*u) + 0.15*std::cos(4.0*v) + 0.1*std::sin(5.0*(u+v));
        }
    }

    std::vector<std::vector<point>> vertices(grid_res_x, std::vector<point>(grid_res_z));
    for (int i = 0; i < grid_res_x; ++i) {
        for (int j = 0; j < grid_res_z; ++j) {
            double x = center.x() - size_x/2 + (size_x * i/(grid_res_x-1));
            double z = center.z() - size_z/2 + (size_z * j/(grid_res_z-1));
            double y = center.y() + heights[i][j];
            vertices[i][j] = point(x, y, z);
        }
    }

    for (int i = 0; i < grid_res_x-1; ++i) {
        for (int j = 0; j < grid_res_z-1; ++j) {
            point p00 = vertices[i][j];
            point p10 = vertices[i+1][j];
            point p01 = vertices[i][j+1];
            point p11 = vertices[i+1][j+1];

            // Triangle 1
            tri_face t1;
            t1.v0 = {p00, vec(0,1,0)};
            t1.v1 = {p10, vec(0,1,0)};
            t1.v2 = {p11, vec(0,1,0)};
            t1.mat = mat;
            faces.push_back(t1);

            // Triangle 2
            tri_face t2;
            t2.v0 = {p00, vec(0,1,0)};
            t2.v1 = {p11, vec(0,1,0)};
            t2.v2 = {p01, vec(0,1,0)};
            t2.mat = mat;
            faces.push_back(t2);
        }
    }
}

bool tri_face::hit(const ray& r, interval ray_t, hit_record& rec) const {
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
    return true;
}

bool rock_floor::hit(const ray& r, interval ray_t, hit_record& rec) const {
    bool hit_any = false;
    double closest = ray_t.max;
    hit_record temp;

    for (const auto& face : faces) {
        if (face.hit(r, interval(ray_t.min, closest), temp)) {
            hit_any = true;
            closest = temp.t;
            rec = temp;
        }
    }

    return hit_any;
}