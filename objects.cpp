#include "incl/objects.h"

sphere::sphere(const point& center, double radius, std::shared_ptr<material> mat)
    : center(center), radius(std::fmax(0, radius)), mat(mat) {}

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = h*h - a*c;
    if (discriminant < 0) return false;

    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;

    return true;
}

void sphere::get_sphere_uv(const point& p, double& u, double& v) const {
    auto theta = std::acos(-p.y());
    auto phi = std::atan2(-p.z(), p.x()) + pi;

    u = phi / (2*pi);
    v = theta / pi;
}

cylinder::cylinder(point center, double radius, double height,
                   std::shared_ptr<material> mat, bool capped)
    : cen(center),
      rad(std::max(0.0, radius)),
      h(height),
      caps(capped),
      mat(mat) {}

bool cylinder::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec d = r.direction();
    vec o = r.origin() - cen;

    double a = d.x()*d.x() + d.z()*d.z();
    double b = 2 * (o.x()*d.x() + o.z()*d.z());
    double c = o.x()*o.x() + o.z()*o.z() - rad*rad;

    double discriminant = b*b - 4*a*c;
    bool hit_anything = false;
    double closest = ray_t.max;

    hit_record temp;

    if (discriminant >= 0) {
        double sqrtd = sqrt(discriminant);

        double root = (-b - sqrtd) / (2*a);
        if (check_cylinder_root(r, root, ray_t.min, closest, temp)) {
            temp.mat = mat;
            rec = temp;
            closest = temp.t;
            hit_anything = true;
        }

        root = (-b + sqrtd) / (2*a);
        if (check_cylinder_root(r, root, ray_t.min, closest, temp)) {
            temp.mat = mat;
            rec = temp;
            closest = temp.t;
            hit_anything = true;
        }
    }

    if (caps) {
        if (intersect_cap(r, 0, ray_t.min, closest, temp)) {
            temp.mat = mat;
            rec = temp;
            closest = temp.t;
            hit_anything = true;
        }
        if (intersect_cap(r, h, ray_t.min, closest, temp)) {
            temp.mat = mat;
            rec = temp;
            closest = temp.t;
            hit_anything = true;
        }
    }

    return hit_anything;
}

bool cylinder::check_cylinder_root(const ray& r, double t, double t_min, double t_max, hit_record& rec) const {
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

bool cylinder::intersect_cap(const ray& r, double y_offset, double t_min, double t_max, hit_record& rec) const {
    double y_plane = cen.y() + y_offset;
    double t = (y_plane - r.origin().y()) / r.direction().y();
    if (t < t_min || t > t_max) return false;

    point p = r.at(t);
    if ((p.x() - cen.x()) * (p.x() - cen.x()) +
        (p.z() - cen.z()) * (p.z() - cen.z()) > rad * rad)
        return false;

    rec.t = t;
    rec.p = p;
    //vec outward_normal = vec(0, (y_offset == 0 ? -1 : 1), 0);
    vec outward_normal = (y_offset == 0 ? vec(0,-1,0) : vec(0,1,0));
    rec.set_face_normal(r, outward_normal);
    get_cap_uv(p, rec.u, rec.v, y_offset == h);
    return true;
}

void cylinder::get_cap_uv(const point& p, double& u, double& v, bool top) const {
    vec rel = p - cen;   
    double x = rel.x();
    double y = rel.y();
    double z = rel.z();

    if (top) y -= h; 

    u = 0.5 + x / (2 * rad);
    v = 0.5 + z / (2 * rad);
}



cone::cone(point tip, double radius, double height,
           std::shared_ptr<material> mat, bool capped)
    : tip(tip),
      rad(std::max(0.0, radius)),
      h(height),
      caps(capped),
      mat(mat) {}

bool cone::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec d = r.direction();
    vec o = r.origin() - tip;

    double k = rad / h;
    double k2 = k * k;

    double a = d.x()*d.x() + d.z()*d.z() - k2 * d.y()*d.y();
    double b = 2 * (o.x()*d.x() + o.z()*d.z() - k2 * o.y()*d.y());
    double c = o.x()*o.x() + o.z()*o.z() - k2 * o.y()*o.y();

    double discriminant = b*b - 4*a*c;
    bool hit_any = false;
    double closest = ray_t.max;
    hit_record tmp;

    if (discriminant >= 0) {
        double sqrtd = sqrt(discriminant);

        double root = (-b - sqrtd) / (2 * a);
        if (check_root(r, root, ray_t.min, closest, tmp)) {
            tmp.mat = mat;
            rec = tmp;
            closest = tmp.t;
            hit_any = true;
        }

        root = (-b + sqrtd) / (2 * a);
        if (check_root(r, root, ray_t.min, closest, tmp)) {
            tmp.mat = mat;
            rec = tmp;
            closest = tmp.t;
            hit_any = true;
        }
    }

    if (caps) {
        double y_plane = tip.y() - h;
        double t = (y_plane - r.origin().y()) / r.direction().y();

        if (t > ray_t.min && t < closest) {
            point p = r.at(t);
            if ((p - point(tip.x(), y_plane, tip.z())).length_squared() <= rad*rad) {
                rec.t = t;
                rec.p = p;
                rec.set_face_normal(r, vec(0, -1, 0));
                rec.mat = mat;
                closest = t;
                hit_any = true;
            }
        }
    }

    return hit_any;
}

bool cone::check_root(const ray& r, double t, double t_min, double t_max, hit_record& rec) const {
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


shape::shape(point center,
                   double radius_bottom,
                   double radius_top,
                   double height,
                   std::shared_ptr<material> mat,
                   bool capped)
    : cen(center),
      rad_bottom(std::max(0.0, radius_bottom)),
      rad_top(std::max(0.0, radius_top)),
      h(height),
      caps(capped),
      mat(mat) {}

bool shape::intersect_cap(const ray& r, double y_offset, double radius_at_y,
                          double t_min, double t_max, hit_record& rec) const {
    double y_plane = cen.y() + y_offset;

    if (std::abs(r.direction().y()) < 1e-12) return false;

    double t = (y_plane - r.origin().y()) / r.direction().y();
    if (t < t_min || t > t_max) return false;

    point p = r.at(t);
    double dx = p.x() - cen.x();
    double dz = p.z() - cen.z();
    if (dx*dx + dz*dz > radius_at_y*radius_at_y + 1e-12) return false;

    rec.t = t;
    rec.p = p;
    vec outward_normal = vec(0, (y_offset == 0 ? -1.0 : 1.0), 0.0);
    rec.set_face_normal(r, outward_normal);
    return true;
}

bool shape::hit(const ray& r, interval ray_t, hit_record& rec) const {
    bool hit_anything = false;
    double closest = ray_t.max;
    hit_record temp;

    vec d = r.direction();
    vec o = r.origin() - cen;
    double k = (rad_top - rad_bottom) / h; // slope of radius / y

    const double EPS = 1e-12;

    if (std::abs(rad_top - rad_bottom) < 1e-8) { // cylinder
        double a = d.x()*d.x() + d.z()*d.z();
        if (a > EPS) {
            double b = 2.0*(o.x()*d.x() + o.z()*d.z());
            double c = o.x()*o.x() + o.z()*o.z() - rad_bottom*rad_bottom;

            double disc = b*b - 4*a*c;
            if (disc >= 0.0) {
                double sq = std::sqrt(disc);
                double roots[2] = { (-b - sq)/(2*a), (-b + sq)/(2*a) };
                for (double t : roots) {
                    if (t < ray_t.min || t > closest) continue;
                    double y = o.y() + t*d.y();
                    if (y < 0.0 || y > h) continue;

                    temp.t = t;
                    temp.p = r.at(t);
                    vec outward_normal = vec(temp.p.x()-cen.x(), 0.0, temp.p.z()-cen.z());
                    temp.set_face_normal(r, unit_vector(outward_normal));
                    temp.mat = mat;

                    closest = t;
                    rec = temp;
                    hit_anything = true;
                }
            }
        }
    }
    else {
        double R0 = rad_bottom + k * o.y();

        double a = d.x()*d.x() + d.z()*d.z() - (k*k) * d.y()*d.y();
        double b = 2.0*(o.x()*d.x() + o.z()*d.z() - R0 * k * d.y());
        double c = o.x()*o.x() + o.z()*o.z() - R0*R0;

        if (std::abs(a) > EPS) {
            double disc = b*b - 4.0*a*c;
            if (disc >= 0.0) {
                double sq = std::sqrt(disc);
                double roots[2] = { (-b - sq)/(2*a), (-b + sq)/(2*a) };
                for (double t : roots) {
                    if (t < ray_t.min || t > closest) continue;
                    point p = r.at(t);
                    double y = p.y() - cen.y();
                    if (y < 0.0 || y > h) continue;

                    double r_y = rad_bottom + k * y;     // radius at point y
                    double dx = p.x() - cen.x();
                    double dz = p.z() - cen.z();
                    if (dx*dx + dz*dz > r_y*r_y + 1e-12) continue;

                    temp.t = t;
                    temp.p = p;
                    vec outward_normal = vec(dx, -k * r_y, dz); // vec(dx, k * r_y, dz);
                    temp.set_face_normal(r, unit_vector(outward_normal));
                    temp.mat = mat;

                    closest = t;
                    rec = temp;
                    hit_anything = true;
                }
            }
        }
        else {
            if (std::abs(b) > EPS) {
                double t = -c / b;
                if (t >= ray_t.min && t <= closest) {
                    point p = r.at(t);
                    double y = p.y() - cen.y();
                    if (!(y < 0.0 || y > h)) {
                        double r_y = rad_bottom + k * y;
                        double dx = p.x() - cen.x();
                        double dz = p.z() - cen.z();
                        if (dx*dx + dz*dz <= r_y*r_y + 1e-12) {
                            temp.t = t;
                            temp.p = p;
                            vec outward_normal = vec(dx, -k * r_y, dz);
                            temp.set_face_normal(r, unit_vector(outward_normal));
                            temp.mat = mat;

                            closest = t;
                            rec = temp;
                            hit_anything = true;
                        }
                    }
                }
            }
        }
    }
    if (caps) {
        if (intersect_cap(r, 0.0, rad_bottom, ray_t.min, closest, temp)) {
            temp.mat = mat;
            closest = temp.t;
            rec = temp;
            hit_anything = true;
        }

        if (intersect_cap(r, h, rad_top, ray_t.min, closest, temp)) {
            temp.mat = mat;
            closest = temp.t;
            rec = temp;
            hit_anything = true;
        }
    }

    return hit_anything;
}

rectangle::rectangle(const point& center, double a, double b, std::shared_ptr<material>mat)
         :  cen(center), b(b), a(a), mat(mat) {}

bool rectangle::hit(const ray& r, interval ray_t, hit_record& rec) const {

    double x0 =  cen.x() - (a/2);
    double x1 =  cen.x() + (a/2);

    double y0 =  cen.y() - (b/2);
    double y1 =  cen.y() + (b/2);

    double k = cen.z();

    double t = (k - r.origin().z()) / r.direction().z();
    if (t < ray_t.min || t > ray_t.max) return false;

    double x = r.origin().x() + t * r.direction().x();
    double y = r.origin().y() + t * r.direction().y();

    if (x < x0 || x > x1 || y < y0 || y > y1) return false;

    rec.t = t;
    rec.p = r.at(t);
    rec.mat = mat;

    vec normal(0, 0, 1);
    rec.set_face_normal(r, normal);

    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);

    return true;
}

triangle::triangle(const point& center, double size, std::shared_ptr<material>mat)
         :  cen(center), s(size), mat(mat) {}

bool triangle::hit(const ray& r, interval ray_t, hit_record& rec) const {
    
    double t = (cen.z() - r.origin().z()) / r.direction().z();
    if (t < ray_t.min || t > ray_t.max) return false;

    double x = r.origin().x() + t * r.direction().x();
    double y = r.origin().y() + t * r.direction().y();

    double xA = A.x(); double yA = A.y();       // A
    double xB = B.x(); double yB = B.y();       // B
    double xC = C.x(); double yC = C.y();        // C

    double den = (yB - yC) * (xA - xC) + (xC - xB) * (yA - yC);

    if (den == 0.0)
        return false;

    double lambdaA = ((yB - yC) * (x - xC) + (xC - xB) * (y - yC)) / den;
    double lambdaB = ((yC - yA) * (x - xC) + (xA - xC) * (y - yC)) / den;
    double lambdaC = 1.0 - lambdaA - lambdaB;

    if (lambdaA >= 0.0 && lambdaB >= 0.0 && lambdaC >= 0.0){
        rec.t = t;
        rec.p = r.at(t);
        rec.mat = mat;

        vec normal(0, 0, 1);
        rec.set_face_normal(r, normal);
        return true;
    }

    return false;

    rec.t = t;
    rec.p = r.at(t);
    rec.mat = mat;

    vec normal(0, 0, 1);
    rec.set_face_normal(r, normal);
    return true;
}

snowflake::snowflake(const point& center, double size, int iteration, std::shared_ptr<material>mat)
         :  cen(center), s(size), it(iteration), mat(mat) {}

std::vector<triangle> snowflake::gener() const {

    std::vector<triangle> triangles;

    triangles.emplace_back(cen, s, mat);

    if ( it == 1 ) return triangles;

    triangle primo = triangles[0];

    std::pair<point, double> AB = where_to_place(primo.A, primo.B);
    std::pair<point, double> AC = where_to_place(primo.A, primo.C);
    std::pair<point, double> CB = where_to_place(primo.C, primo.B);

    triangles.emplace_back(AB.first, AB.second, mat);
    triangles.emplace_back(AC.first, AC.second, mat);
    triangles.emplace_back(CB.first, CB.second, mat);
    
    return triangles;
}

point snowflake::halfway( point Q, point W ) const {

    point E = point( (Q.x() - W.x()) / 2, (Q.y() - W.y()) / 2, Q.z() ) ;

    return E;
}

std::pair<point, double> snowflake::where_to_place( point Q, point W ) const {

    point N;
    point half_p = halfway(Q, W);
    double QW = sqrt( (W.x() - Q.x() ) * ( W.x() - Q.x() ) + ( W.y() - Q.y() ) * ( W.y() - Q.y() ));
    double qw = QW / 3.0;
    double h_t = sqrt(3) * qw / 2;

    if ( Q.x() == W.x() ) {
        N = point( half_p.x(), half_p.y() + h_t, Q.z() );
        return std::make_pair(N, qw);
    }

    double coeff_a = ( W.y() - Q.y() ) / ( W.x() - Q.x() );

    if ( coeff_a > 0 ) {
       N = point( half_p.x() + ( sqrt(3) * h_t / 2 ), half_p.y() - ( h_t / 2 ), Q.z() );
       return std::make_pair(N, qw);
    }

    N = point( half_p.x() - ( sqrt(3) * h_t / 2 ), half_p.y() - ( h_t / 2 ), Q.z() );
    return std::make_pair(N, qw);        
}

bool snowflake::hit(const ray& r, interval ray_t, hit_record& rec) const {

    std::vector<triangle> fig = gener();

    for (const triangle& triang : fig){
        if (triang.hit(r, ray_t, rec) ) return true;
    }
    return false;
}
