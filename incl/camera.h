#pragma once
#include "vec.h"
#include "color.h"
#include "sdl_handler.h"

class camera {
  public:
    IMAGE image = {1000, 16.0 / 9.0}; //width, ratio
    //SDL_IMAGE sdl_image = initiate_image(image); //create sdl window

    int samples_per_pixel = 50;
    int max_depth =20;

    double vfov = 90; //vertical view angle
    point lookfrom = point(0,0,0);
    point lookat = point(0,0,-1);
    vec vup = vec(0,1,0);  

    double defocus_angle = 0;
    double focus_dist = 10;    //point the camera is focused on

    double linear_to_gamma(double linear_comp){
        if (linear_comp > 0)
          return std::sqrt(linear_comp);
        return 0;
    }

    void render(const hittable& world, std::vector<uint8_t>& pixels) {
        initialize();
        //std::vector<unsigned char> pixels(image.width * image.height * 3);

        for (int j = 0; j < image.height; j++) {
            for (int i = 0; i < image.width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                pixel_color *= pixel_samples_scale;
                int index = (j * image.width + i) * 3;

                auto r = linear_to_gamma(pixel_color.x()); // red
                auto g = linear_to_gamma(pixel_color.y()); // green
                auto b = linear_to_gamma(pixel_color.z()); // blue

                static const interval intensity(0.000, 0.999);
                pixels[index] = static_cast<int>(256 * intensity.clamp(r));
                pixels[index + 1] = static_cast<int>(256 * intensity.clamp(g));
                pixels[index + 2] = static_cast<int>(256 * intensity.clamp(b));
            }
        }
        //SDL_UpdateTexture(sdl_image.texture, nullptr, pixels.data(), image.width * 3);
    }

  private:
    double pixel_samples_scale;
    point center;
    point pixel00_loc;    // Location of pixel 0, 0
    vec pixel_delta_u;  // Offset to pixel to the right
    vec pixel_delta_v;  // Offset to pixel below
    vec u, v, w;     
    vec defocus_disk_u;
    vec defocus_disk_v;

    void initialize() {
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = lookfrom;
        //center = point(1, 0.5, 0.5);

        //auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image.width)/image.height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        vec viewport_u = viewport_width * u; 
        vec viewport_v = viewport_height * -v; 

        pixel_delta_u = viewport_u / image.width;
        pixel_delta_v = viewport_v / image.height;

        //start point of the rendering
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec sample_square() const {
        return vec(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};
