#pragma once
#include "vec.h"
#include "color.h"
#include "sdl_handler.h"

class camera {
  public:
    IMAGE image = {1000, 16.0 / 9.0}; //width, ratio
    SDL_IMAGE sdl_image = initiate_image(image); //create sdl window

    void render(const hittable& world) {
        initialize();
        std::vector<unsigned char> pixels(image.width * image.height * 3);

        for (int j = 0; j < image.height; j++) {
            for (int i = 0; i < image.width; i++) {
                auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
                auto ray_direction = pixel_center - center;
                ray r(center, ray_direction);
                color pixel_color = ray_color(r, world);

                int index = (j * image.width + i) * 3;
                pixels[index] = static_cast<int>(255.999 * pixel_color.x()); // red
                pixels[index + 1] = static_cast<int>(255.999 * pixel_color.y()); // grenn
                pixels[index + 2] = static_cast<int>(255.999 * pixel_color.z()); // blue
            }
        }
        SDL_UpdateTexture(sdl_image.texture, nullptr, pixels.data(), image.width * 3);
    }

  private:
    point center;
    point pixel00_loc;    // Location of pixel 0, 0
    vec pixel_delta_u;  // Offset to pixel to the right
    vec pixel_delta_v;  // Offset to pixel below

    void initialize() {
        center = point(0, 0, 0);

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
};