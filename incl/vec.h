#pragma once
#include "somewhere.h"

class vec {
  public:
    double elem[3];

    vec() : elem{0,0,0} {}
    vec(double e0, double e1, double e2) : elem{e0, e1, e2} {}

    double x() const { return elem[0]; }
    double y() const { return elem[1]; }
    double z() const { return elem[2]; }

    vec operator-() const { return vec(-elem[0], -elem[1], -elem[2]); }
    double operator[](int i) const { return elem[i]; }
    double& operator[](int i) { return elem[i]; }

    vec& operator+=(const vec& v) {
        elem[0] += v.elem[0];
        elem[1] += v.elem[1];
        elem[2] += v.elem[2];
        return *this;
    }

    vec& operator*=(double t) {
        elem[0] *= t;
        elem[1] *= t;
        elem[2] *= t;
        return *this;
    }

    vec& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return std::sqrt(elem[0]*elem[0] + elem[1]*elem[1] + elem[2]*elem[2]);
    }

    double length_squared() const {
        return elem[0]*elem[0] + elem[1]*elem[1] + elem[2]*elem[2];
    }

    static vec random(){
      return vec(rand_doub(), rand_doub(), rand_doub());
    }
    
    static vec random(double min, double max){
      return vec(rand_doub(min, max), rand_doub(min, max), rand_doub(min, max));
    }
};

using point = vec;
using color = vec;

inline std::ostream& operator<<(std::ostream& out, const vec& v) {
    return out << v.elem[0] << ' ' << v.elem[1] << ' ' << v.elem[2];
}

inline vec operator+(const vec& u, const vec& v) {
    return vec(u.elem[0] + v.elem[0], u.elem[1] + v.elem[1], u.elem[2] + v.elem[2]);
}

inline vec operator-(const vec& u, const vec& v) {
    return vec(u.elem[0] - v.elem[0], u.elem[1] - v.elem[1], u.elem[2] - v.elem[2]);
}

inline vec operator*(const vec& u, const vec& v) {
    return vec(u.elem[0] * v.elem[0], u.elem[1] * v.elem[1], u.elem[2] * v.elem[2]);
}

inline vec operator*(double t, const vec& v) {
    return vec(t*v.elem[0], t*v.elem[1], t*v.elem[2]);
}

inline vec operator*(const vec& v, double t) {
    return t * v;
}

inline vec operator/(const vec& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec& u, const vec& v) {
    return u.elem[0] * v.elem[0]
         + u.elem[1] * v.elem[1]
         + u.elem[2] * v.elem[2];
}

inline vec cross(const vec& u, const vec& v) {
    return vec(u.elem[1] * v.elem[2] - u.elem[2] * v.elem[1],
                u.elem[2] * v.elem[0] - u.elem[0] * v.elem[2],
                u.elem[0] * v.elem[1] - u.elem[1] * v.elem[0]);
}

inline vec unit_vector(const vec& v) {
    return v / v.length();
}

inline vec random_unit_vector() {
  while (true){
    auto p= vec::random(-1, 1);
    auto len_sq=p.length();
    if (1e-160 < len_sq && len_sq <= 1){
      return p/sqrt(len_sq);
    }
  }
}

inline vec random_on_hemisphere(const vec& normal) {
    vec on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}
