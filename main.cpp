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

    cam.lookfrom = point( -1, 3.0, -3.3);
    cam.lookat = point( -1, 2.5, 0);
    cam.vup = vec( 0, 1, 0);
    cam.vfov = 70;

    cam.defocus_angle = 0.4;
    cam.focus_dist = (cam.lookfrom - cam.lookat).length(); // or 2.5

    hittable_list world;

    auto ocean = std::make_shared<image_texture>("ocean.png");
    auto ocean_material = std::make_shared<lambertian>(ocean);

    auto rock = std::make_shared<image_texture>("rock.png");
    auto rock_material = std::make_shared<lambertian>(rock);

    auto striped = std::make_shared<striped_texture>(0.32, color(1, .1, .1), color(.9, .9, .9));
    auto striped_material = std::make_shared<lambertian>(striped);

    auto material_ground = std::make_shared<lambertian>(color(0.1, 0.3, 0.8));
    auto material_center = std::make_shared<lambertian>(color(0.9, 0.3, 0.2));
    auto red = std::make_shared<lambertian>(color(1, 0.1, 0.1));
    //auto rock = std::make_shared<lambertian>(color(0.3, 0.3, 0.3));
    auto glass   = std::make_shared<dielectric>(1.3);
    auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = std::make_shared<metal>(color(0.9, 0.3, 0.2), 0.05);


    world.add(std::make_shared<cylinder>(point(0,-1,0), 25.0, 0.5, ocean_material)); //the ocean

    world.add(std::make_shared<rock_floor>(point(0, 0, 0), 5, 2, 10, 10, rock_material));// rocky floor

    //main shape
    world.add(std::make_shared<shape>(point(0, 0, 0), 0.6, 0.3, 2.5, striped_material, true)); // base
    world.add(std::make_shared<cylinder>(point(0, 2.5, 0), 0.3, 0.7, striped_material)); // base
    world.add(std::make_shared<cylinder>(point(0, 2.9, 0), 0.31, 0.3, glass, false)); // window
    //world.add(std::make_shared<cone>(point(0, 3.5, 0), 0.43, 0.35 , red)); // roof
    world.add(std::make_shared<shape>(point(0, 3.15, 0), 0.43, 0.0, 0.35 , red, true)); // roof

    // //details
    world.add(std::make_shared<sphere>(point( 0, 3.5, 0), 0.04, red)); // sphere on top
    world.add(std::make_shared<cylinder>(point(0, 2.5, 0), 0.5, 0.05, red)); // balcony
    world.add(std::make_shared<cone>(point(0, 3.7, 0), 0.01, 0.3 , red)); // point on top


    world.add(std::make_shared<cylinder>(point(0, 2.7, 0), 0.5, 0.02, red, false)); // railings
    world.add(std::make_shared<cylinder>(point(0, 2.8, 0), 0.5, 0.02, red, false));
    world.add(std::make_shared<cylinder>(point(0.5, 2.525, 0), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(-0.5, 2.525, 0), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(0, 2.525, 0.5), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(0, 2.525, -0.5), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(0.35, 2.525, 0.35), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(0.35, 2.525, -0.35), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(-0.35, 2.525, 0.35), 0.015, 0.3, red));
    world.add(std::make_shared<cylinder>(point(-0.35, 2.525, -0.35), 0.015, 0.3, red));

    

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



