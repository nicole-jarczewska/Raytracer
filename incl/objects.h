#pragma once
#include "hit.h"

class sphere : public hittable {
  public:
    sphere(const point& center, double radius) : center(center), radius(std::fmax(0,radius)) {}

    bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
        vec oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        auto root = (h - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax <= root) {
            root = (h + sqrtd) / a;
            if (root <= ray_tmin || ray_tmax <= root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

  private:
    point center;
    double radius;
};

class cylinder : public hittable {
public:
    cylinder(point center, double radius, double height, bool capped = true)
        : cen(center), rad(radius), h(height), caps(capped) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec d = r.direction();
        vec o = r.origin() - cen;

        //x^2 + z^2 = r^2
        double a = d.x()*d.x() + d.z()*d.z();
        double b = 2 * (o.x()*d.x() + o.z()*d.z());
        double c = o.x()*o.x() + o.z()*o.z() - rad*rad;

        double discriminant = b*b - 4*a*c;
        bool hit_anything = false;
        double best_t = t_max;
        hit_record temp_rec;

        if (discriminant >= 0) {
            double sqrtd = sqrt(discriminant);
            double root = (-b - sqrtd) / (2*a);
            if (check_cylinder_root(r, root, t_min, best_t, temp_rec))
                { best_t = temp_rec.t; rec = temp_rec; hit_anything = true; }

            root = (-b + sqrtd) / (2*a);
            if (check_cylinder_root(r, root, t_min, best_t, temp_rec))
                { best_t = temp_rec.t; rec = temp_rec; hit_anything = true; }
        }

        if (caps) {
            double t_cap;
            if (intersect_cap(r, 0, t_min, best_t, temp_rec)) {
                best_t = temp_rec.t; rec = temp_rec; hit_anything = true;
            }
            if (intersect_cap(r, h, t_min, best_t, temp_rec)) {
                best_t = temp_rec.t; rec = temp_rec; hit_anything = true;
            }
        }

        return hit_anything;
    }

private:
    point cen;
    double rad;
    double h;
    bool caps;

    bool check_cylinder_root(const ray& r, double t, double t_min, double t_max, hit_record& rec) const {
        if (t < t_min || t > t_max) return false;
        point p = r.at(t);
        double y = p.y() - cen.y();
        if (y < 0 || y > h) return false;

        rec.t = t;
        rec.p = p;
        vec outward_normal = vec(p.x() - cen.x(), 0, p.z() - cen.z()) / rad;
        rec.set_face_normal(r, outward_normal);
        return true;
    }

    bool intersect_cap(const ray& r, double y_offset, double t_min, double t_max, hit_record& rec) const {
        double y_plane = cen.y() + y_offset;
        double t = (y_plane - r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max) return false;

        point p = r.at(t);
        if ((p.x() - cen.x())* (p.x() - cen.x()) +
            (p.z() - cen.z())* (p.z() - cen.z()) > rad*rad)
            return false;

        rec.t = t;
        rec.p = p;
        vec outward_normal = vec(0, (y_offset == 0 ? -1 : 1), 0);
        rec.set_face_normal(r, outward_normal);
        return true;
    }
};


class cone : public hittable {
public:
    cone(point tip, double radius, double height, bool capped = true)
        : tip(tip), rad(radius), h(height), caps(capped) {}

    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        vec d = r.direction();
        vec o = r.origin() - tip;

        double k = rad / h;
        double k2 = k*k;

        double a = d.x()*d.x() + d.z()*d.z() - k2 * d.y()*d.y();
        double b = 2 * (o.x()*d.x() + o.z()*d.z() - k2 * o.y()*d.y());
        double c = o.x()*o.x() + o.z()*o.z() - k2 * o.y()*o.y();

        double discriminant = b*b - 4*a*c;
        bool hit_any = false;
        double t_best = t_max;
        hit_record temp_rec;

        if (discriminant >= 0) {
            double sqrtd = sqrt(discriminant);

            double root = (-b - sqrtd) / (2*a);
            if (check_root(r, root, t_min, t_best, temp_rec)) {
                t_best = temp_rec.t; rec = temp_rec; hit_any = true;
            }

            root = (-b + sqrtd) / (2*a);
            if (check_root(r, root, t_min, t_best, temp_rec)) {
                t_best = temp_rec.t; rec = temp_rec; hit_any = true;
            }
        }

        if (caps) {
            double y_plane = tip.y() - h;
            double t = (y_plane - r.origin().y()) / r.direction().y();
            if (t > t_min && t < t_best) {
                point p = r.at(t);
                if ((p - point(tip.x(), y_plane, tip.z())).length_squared() <= rad*rad) {
                    rec.t = t;
                    rec.p = p;
                    rec.set_face_normal(r, vec(0, -1, 0));
                    hit_any = true;
                }
            }
        }

        return hit_any;
    }

private:
    point tip;
    double rad, h;
    bool caps;

    bool check_root(const ray& r, double t, double t_min, double t_max, hit_record& rec) const {
        if (t < t_min || t > t_max) return false;

        point p = r.at(t);
        double y = tip.y() - p.y();
        if (y < 0 || y > h) return false;

        rec.t = t;
        rec.p = p;

        vec v = p - tip;
        vec normal(v.x(), h * rad / (h - y), v.z());
        rec.set_face_normal(r, unit_vector(normal));

        return true;
    }
};
