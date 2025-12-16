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

    // x + ---> w prawo, x - ---> w lewo
    // y + ---> w góre, y - ---> w dół
    // z + ---> do przodu, z - ---> do tyłu

int main() {
    ////camera settings
    camera cam;
    cam.background = color(0.8, 0.85, 1.0); //blue sky
    //cam.background = color(0.0, 0.0, 0.0);

    cam.samples_per_pixel = 25;
    cam.max_depth = 15;

    cam.lookfrom = point(-0.9, 3.0, -3.3);
    cam.lookat = point(-0.9, 2.5, 0);
    cam.vup = vec( 0, 1, 0);
    cam.vfov = 70;

    cam.defocus_angle = 0.4;
    cam.focus_dist = (cam.lookfrom - cam.lookat).length(); // or 2.5

    hittable_list world;


    ////colors and textures
    auto ocean = std::make_shared<image_texture>("assets/ocean.png");
    auto ocean_material = std::make_shared<lambertian>(ocean);

    auto rock = std::make_shared<image_texture>("assets/rock.png");
    auto rock_material = std::make_shared<lambertian>(rock);

    auto striped = std::make_shared<striped_texture>(0.32, color(1, .1, .1), color(.9, .9, .9));
    auto striped_material = std::make_shared<lambertian>(striped);

    auto light = std::make_shared<diffuse_light>(color(4,4,4));
    auto sunlight = std::make_shared<diffuse_light>(color(10,10,10));

    auto material_ground = std::make_shared<lambertian>(color(0.1, 0.3, 0.8));
    auto material_center = std::make_shared<lambertian>(color(0.9, 0.3, 0.2));
    auto red = std::make_shared<lambertian>(color(1, 0.1, 0.1));
    
    //lanter
    auto mat_red = std::make_shared<metal>(color(0.9, 0.1, 0.1), 0.1);
    auto mat_white = std::make_shared<metal>(color(1, 1, 1), 0.1);
    
    //auto rock = std::make_shared<lambertian>(color(0.3, 0.3, 0.3));
    auto glass1 = std::make_shared<dielectric>(1.5);
    auto blue_glass = std::make_shared<glass>(1.5, color(0.8, 0.85, 1));
    auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50);
    auto metal_material  = std::make_shared<metal>(color(0.8, 0.8, 0.8), 0.01);
  
    //snowflake
    world.add(std::make_shared<snowflake>(point(-3,1.5,0), 2, 3, mat_red));

    ////the scene

    world.add(std::make_shared<cylinder>(point(0,-1,0), 35.0, 0.5, ocean_material)); //the ocean

    //world.add(std::make_shared<rock_floor>(point(0, 0, 0), 5, 2, 10, 10, rock_material));// rocky floor
    float start_point_x = 0.0;
    float start_point_z = 0.0;
    //main shape
    //world.add(std::make_shared<shape>(point(start_point_x, 0, start_point_z), 0.6, 0.3, 2.5, striped_material, true)); // base
    
    world.add(std::make_shared<shape>(point(start_point_x, -0.15, start_point_z), 0.56, 0.53, 0.3, mat_white, true));
    world.add(std::make_shared<shape>(point(start_point_x, 0.15, start_point_z), 0.53, 0.5, 0.3, mat_red, true));
    world.add(std::make_shared<shape>(point(start_point_x, 0.45, start_point_z), 0.5, 0.47, 0.3, mat_white, true));
    world.add(std::make_shared<shape>(point(start_point_x, 0.75, start_point_z), 0.47, 0.44, 0.3, mat_red, true));
    world.add(std::make_shared<shape>(point(start_point_x, 1.05, start_point_z), 0.44, 0.41, 0.3, mat_white, true));
    world.add(std::make_shared<shape>(point(start_point_x, 1.35, start_point_z), 0.41, 0.39, 0.3, mat_red, true));
    world.add(std::make_shared<shape>(point(start_point_x, 1.65, start_point_z), 0.39, 0.36, 0.3, mat_white, true));
    world.add(std::make_shared<shape>(point(start_point_x, 1.95, start_point_z), 0.36, 0.33, 0.3, mat_red, true));
    world.add(std::make_shared<shape>(point(start_point_x, 2.25, start_point_z), 0.33, 0.3, 0.3, mat_white, true));
    
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.5, start_point_z), 0.3, 0.4, mat_red, false)); // base
    world.add(std::make_shared<cylinder>(point(start_point_x, 3.1, start_point_z), 0.3, 0.05, mat_red, false)); // base
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.9, start_point_z), 0.29, 0.2, blue_glass , true)); // window
    //world.add(std::make_shared<cone>(point(0, 3.5, 0), 0.43, 0.35 , red)); // roof
    world.add(std::make_shared<shape>(point(start_point_x, 3.15, start_point_z), 0.43, 0.0, 0.35 , mat_red, true)); // roof


    // //details
    world.add(std::make_shared<sphere>(point(start_point_x, 3.5, start_point_z), 0.04, mat_red)); // sphere on top
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.55, start_point_z), 0.52, 0.05, mat_red)); // balcony
    world.add(std::make_shared<shape>(point(start_point_x, 3.5, start_point_z), 0.01, 0.0, 0.3, mat_red, true)); // point on top
    world.add(std::make_shared<cylinder>(point(start_point_x + 0.29, 2.9, start_point_z), 0.01, 0.2, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x - 0.29, 2.9, start_point_z), 0.01, 0.2, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.9, start_point_z + 0.29), 0.01, 0.2, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.9, start_point_z - 0.29), 0.01, 0.2, mat_red));

    // railings
    float railing_h = 2.55;
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.70, start_point_z), 0.5, 0.02, mat_red, false));
    world.add(std::make_shared<cylinder>(point(start_point_x, 2.83, start_point_z), 0.5, 0.02, mat_red, false));
    world.add(std::make_shared<cylinder>(point(start_point_x + 0.5, railing_h, start_point_z), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x - 0.5, railing_h, start_point_z), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x, railing_h, start_point_z + 0.5), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x, railing_h, start_point_z - 0.5), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x + 0.35, railing_h, start_point_z + 0.35), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x + 0.35, railing_h, start_point_z - 0.35), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x - 0.35, railing_h, start_point_z + 0.35), 0.015, 0.3, mat_red));
    world.add(std::make_shared<cylinder>(point(start_point_x - 0.35, railing_h, start_point_z - 0.35), 0.015, 0.3, mat_red));

    //world.add(std::make_shared<sphere>(point(-1.0,    0.0, -1.0),   0.4, material_bubble));
    //world.add(std::make_shared<sphere>(point( 1.0,    0.0, -1.0),   0.5, material_right));
    
    
    world.add(std::make_shared<sphere>(point(1.0, 2.0, -1.0),   0.5, material_bubble));
    world.add(std::make_shared<sphere>(point(1.0, 2.0, -1.0),   0.7, glass1));
    //world.add(std::make_shared<sphere>(point(-3.0, 2.0, -1.0),   0.9, metal_material));

    world.add(std::make_shared<sphere>(point(start_point_x, 2.95 ,start_point_z), 0.07, light));
    world.add(std::make_shared<sphere>(point(-10, 10, 20), 2, sunlight));


    IMAGE img(1000, 16.0 / 9.0);
    std::vector<uint8_t> pixels(img.width * img.height * 3);
    cam.render(world, pixels);
    OutputMode mode = OutputMode::PPM_FILE;
    output_image(mode, img, pixels, "snowflake_wip.ppm");
    //handle_image(cam.sdl_image);

    return 0;
}
