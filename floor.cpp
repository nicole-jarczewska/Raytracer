#include "incl/objects.h"

rock_floor::rock_floor(const point& center,
                       double size_x,
                       double size_z,
                       int grid_res_x,
                       int grid_res_z,
                       std::shared_ptr<material> mat,
                       double texture_repeat)
{
    std::vector<std::vector<double>> heights(grid_res_x, std::vector<double>(grid_res_z, 0.0));

    for (int i = 0; i < grid_res_x; ++i) {
        for (int j = 0; j < grid_res_z; ++j) {
            double u = static_cast<double>(i)/(grid_res_x-1);
            double v = static_cast<double>(j)/(grid_res_z-1);
            heights[i][j] = 0.2*std::sin(3.0*u) + 0.15*std::cos(4.0*v) + 0.1*std::sin(5.0*(u+v));
        }
    }

    std::vector<std::vector<vertex>> vertices(grid_res_x, std::vector<vertex>(grid_res_z));

    for (int i = 0; i < grid_res_x; ++i) {
        for (int j = 0; j < grid_res_z; ++j) {
            double x = center.x() - size_x/2 + (size_x * i/(grid_res_x-1));
            double z = center.z() - size_z/2 + (size_z * j/(grid_res_z-1));
            double y = center.y() + heights[i][j];

            double u = texture_repeat * static_cast<double>(i)/(grid_res_x-1);
            double v = texture_repeat * static_cast<double>(j)/(grid_res_z-1);

            vertices[i][j] = vertex{point(x,y,z), vec(0,1,0), u, v};
        }
    }

    for (int i = 0; i < grid_res_x-1; ++i) {
        for (int j = 0; j < grid_res_z-1; ++j) {
            // Triangle 1
            tri_face t1;
            t1.v0 = vertices[i][j];
            t1.v1 = vertices[i+1][j];
            t1.v2 = vertices[i+1][j+1];
            t1.mat = mat;
            faces.push_back(t1);

            // Triangle 2
            tri_face t2;
            t2.v0 = vertices[i][j];
            t2.v1 = vertices[i+1][j+1];
            t2.v2 = vertices[i][j+1];
            t2.mat = mat;
            faces.push_back(t2);
        }
    }
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