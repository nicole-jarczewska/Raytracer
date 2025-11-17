#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <string>
 // #include "incl/libraries.h"
#include "objects.cpp"
#include "floor.cpp"
#include "incl/camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "incl/stb_image.h"

int main() {
    camera cam;
    // x + ---> w prawo, x - ---> w lewo
    // y + ---> w góre, y - ---> w dół
    // z + ---> do przodu, z - ---> do tyłu
    cam.samples_per_pixel = 25;
    cam.max_depth = 15;

    cam.lookfrom = point( -1, 2.5, -2.5);
    cam.lookat = point( -1, 2.5, 0);
    cam.vup = vec( 0, 1, 0);
    cam.vfov = 100;

    cam.defocus_angle = 0.6;
    cam.focus_dist = 2;

    hittable_list world;

    auto ocean = std::make_shared<image_texture>("ocean.png");
    if (!ocean->is_valid()) {
    std::cerr << "Failed to load texture!" << std::endl;
    }
    auto ocean_material = std::make_shared<lambertian>(ocean);

    auto striped = std::make_shared<striped_texture>(0.32, color(.9, .3, .1), color(.9, .9, .9));
    auto striped_material = std::make_shared<lambertian>(striped);

    auto material_ground = std::make_shared<lambertian>(color(0.1, 0.3, 0.8));
    auto material_center = std::make_shared<lambertian>(color(0.9, 0.3, 0.2));
    auto rock = std::make_shared<lambertian>(color(0.9, 0.8, 0.9));
    auto glass   = std::make_shared<dielectric>(1.5);
    auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = std::make_shared<metal>(color(0.9, 0.3, 0.2), 0.05);


    world.add(std::make_shared<cylinder>(point(0,-1,0), 30.0, 0.5, ocean_material)); //the ocean

    world.add(std::make_shared<rock_floor>(point(0, 0, 0), 5, 2, 10, 10, rock));// rocky floor

    //main shape
    world.add(std::make_shared<shape>(point(0, 0, 0), 0.6, 0.3, 2.5, striped_material, true));
    world.add(std::make_shared<cylinder>(point(0, 2.5, 0), 0.3, 0.7, striped_material));
    world.add(std::make_shared<cylinder>(point(0, 2.9, 0), 0.31, 0.3, glass, false));
    world.add(std::make_shared<cone>(point(0, 3.5, 0), 0.43, 0.35 , material_center));

    // //details
    world.add(std::make_shared<sphere>(point( 0, 3.5, 0), 0.04, material_center));
    world.add(std::make_shared<cylinder>(point(0, 2.5, 0), 0.5, 0.05, material_center));
    world.add(std::make_shared<cone>(point(0, 3.7, 0), 0.01, 0.3 , material_center));

    //world.add(std::make_shared<sphere>(point(-1.0,    0.0, -1.0),   0.4, material_bubble));
    //world.add(std::make_shared<sphere>(point( 1.0,    0.0, -1.0),   0.5, material_right));


    IMAGE img(1000, 16.0 / 9.0);
    std::vector<uint8_t> pixels(img.width * img.height * 3);
    cam.render(world, pixels);
    OutputMode mode = OutputMode::PPM_FILE;
    output_image(mode, img, pixels, "render.ppm");
    //handle_image(cam.sdl_image);

    return 0;
}



