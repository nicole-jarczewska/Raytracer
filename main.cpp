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

    cam.lookfrom = point(1,1,0);
    cam.lookat = point(0,0,-1);
    cam.vup = vec(0,1,0);
    cam.vfov = 20;

    hittable_list world;

    //world.add(std::make_shared<sphere>(point(1,0,-1), 0.7)); // test square // where, size
    world.add(std::make_shared<cylinder>(point(0,0,-1), 0.1, 0.2, true));
    world.add(std::make_shared<cone>(point(1, 0.4,-1), 0.4, 0.7, true));
    //world.add(std::make_shared<cone>(point(1, 2, -3), 1.0, 2.0, true));
    //world.add(std::make_shared<sphere>(point(0,-100.5,-1), 100)); //background

    cam.render(world);
    handle_image(cam.sdl_image);

    return 0;
}

