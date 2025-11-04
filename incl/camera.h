#pragma once
#include "vec.h"
#include "color.h"
#include "sdl_handler.h"

class camera {
  public:
    IMAGE image = {1000, 16.0 / 9.0}; //width, ratio
    SDL_IMAGE sdl_image = initiate_image(image); //create sdl window
    int samples_per_pixel = 10;
    int max_depth = 10;

    double linear_to_gamma(double linear_comp){
        if (linear_comp > 0)
          return std::sqrt(linear_comp);
        return 0;
    }

    void render(const hittable& world) {
        initialize();
        std::vector<unsigned char> pixels(image.width * image.height * 3);

        for (int j = 0; j < image.height; j++) {
            for (int i = 0; i < image.width; i++) {
                color pixel_color(0,0,0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                pixel_color *= pixel_samples_scale;
                int index = (j * image.width + i) * 3;
                pixels[index]=linear_to_gamma(pixels[index]); // red
                pixels[index + 1]=linear_to_gamma(pixels[index + 1]); // green
                pixels[index + 2]=linear_to_gamma(pixels[index + 2]); // blue
                static const interval intensity(0.000, 0.999);
                pixels[index] = static_cast<int>(256 * intensity.clamp(pixel_color.x())); // red
                pixels[index + 1] = static_cast<int>(256 * intensity.clamp(pixel_color.y())); // grenn
                pixels[index + 2] = static_cast<int>(256 * intensity.clamp(pixel_color.z())); // blue
            }
        }
        SDL_UpdateTexture(sdl_image.texture, nullptr, pixels.data(), image.width * 3);
    }

  private:
    double pixel_samples_scale;
    point center;
    point pixel00_loc;    // Location of pixel 0, 0
    vec pixel_delta_u;  // Offset to pixel to the right
    vec pixel_delta_v;  // Offset to pixel below

    void initialize() {
        pixel_samples_scale = 1.0 / samples_per_pixel;
        center = point(1, 0.5, 0.5); // x,y,z
        // x + ---> w prawo, x - ---> w lewo
        // y + ---> w góre, y - ---> w dół
        // z + ---> do przodu, z - ---> do tyłu

        // Determine viewport dimensions.
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image.width)/image.height);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = vec(viewport_width, 0, 0);
        auto viewport_v = vec(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image.width;
        pixel_delta_v = viewport_v / image.height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - vec(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec sample_square() const {
        return vec(random_double() - 0.5, random_double() - 0.5, 0);
    }
};
