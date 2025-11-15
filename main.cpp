#include <iostream>
#include "incl/libraries.h"
// using namespace std;
#include "sdltamplate.h"


// int main(){
//     int width=800;
//     int height=400;
//     sdltemplate::sdl("Ray Tracer", width, height);
//     sdltemplate::loop();
//     for (int y=height-1;y>=0;y--){
//         for(int x=0; x<width; x++){
//             float r= float(x)/ float(width);
//             float g= float(y)/ float(height);
//             float b= 0.2;
//             int ir=int(255.99*r);
//             int ig=int(255.99*g);
//             int ib=int(255.99*b);
//             sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
//             sdltemplate::drawPoint(x, height-y);
//         }
//     }
//     while(sdltemplate::running){
//         sdltemplate::loop();
//     }

//     return 0;
// }

int main() {
    camera cam;
    // x + ---> w prawo, x - ---> w lewo
    // y + ---> w góre, y - ---> w dół
    // z + ---> do przodu, z - ---> do tyłu
    cam.samples_per_pixel = 25;
    cam.max_depth = 15;

    cam.lookfrom = point( 0, 0, -2);
    cam.lookat = point( 0, 0, -1);
    cam.vup = vec( 0, 1, 0);
    cam.vfov = 100;

    cam.defocus_angle = 0.6;
    cam.focus_dist = 1.5;

    hittable_list world;

    auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<dielectric>(1.5);
    auto material_bubble = std::make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2));

    //world.add(std::make_shared<sphere>(point(1, 0,-1), 0.5)); // test square // where, size
    //world.add(std::make_shared<cylinder>(point(0,0,-1.5), 0.1, 0.2, true));
    //world.add(std::make_shared<cone>(point(1, 0.4,-1), 0.4, 0.7, true));
    //world.add(std::make_shared<cone>(point(1, 2, -3), 1.0, 2.0, true));
    world.add(std::make_shared<sphere>(point(0,-100.5,-1), 100, material_ground)); //background

    world.add(std::make_shared<sphere>(point( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(std::make_shared<sphere>(point(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<sphere>(point(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(std::make_shared<sphere>(point( 1.0,    0.0, -1.0),   0.5, material_right));

    cam.render(world);
    handle_image(cam.sdl_image);

    return 0;
}



