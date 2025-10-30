#include <iostream>
#include "incl/libraries.h"
// using namespace std;
#include "sdltamplate.h"


int main(){
    int width=800;
    int height=400;
    sdltemplate::sdl("Ray Tracer", width, height);
    sdltemplate::loop();
    for (int y=height-1;y>=0;y--){
        for(int x=0; x<width; x++){
            float r= float(x)/ float(width);
            float g= float(y)/ float(height);
            float b= 0.2;
            int ir=int(255.99*r);
            int ig=int(255.99*g);
            int ib=int(255.99*b);
            sdltemplate::setDrawColor(sdltemplate::createColor(ir, ig, ib, 255));
            sdltemplate::drawPoint(x, height-y);
        }
    }
    while(sdltemplate::running){
        sdltemplate::loop();
    }

    return 0;
}

// int main() {
//     IMAGE image = {1000, 16.0 / 9.0};

//     hittable_list world;

//     world.add(std::make_shared<sphere>(point(0,0,-1), 0.5));
//     world.add(std::make_shared<sphere>(point(0,-100.5,-1), 100));

//     auto focal_length = 1.0;
//     auto viewport_height = 2.0;
//     auto viewport_width = viewport_height * (double(image.width) / image.height);
//     point camera_center = point(0,0,0);

//     vec viewport_u = vec(viewport_width, 0, 0);
//     vec viewport_v = vec(0, -viewport_height, 0);

//     vec pixel_delta_u = viewport_u / image.width;
//     vec pixel_delta_v = viewport_v / image.height;

//     vec viewport_upper_left =
//         camera_center - vec(0, 0, focal_length) - viewport_u/2 - viewport_v/2;

//     vec pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    
//     SDL_IMAGE sdl_image = initiate_image(image);

//     std::vector<unsigned char> pixels(image.width * image.height * 3);

//     // render
//     for (int j = 0; j < image.height; j++) {
//         for (int i = 0; i < image.width; i++) {
//             auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
//             ray r(camera_center, pixel_center - camera_center);
//             color pixel_color = ray_color(r, world);

//             int index = (j * image.width + i) * 3;
//             pixels[index] = static_cast<int>(255.999 * pixel_color.x()); // red
//             pixels[index + 1] = static_cast<int>(255.999 * pixel_color.y()); // grenn
//             pixels[index + 2] = static_cast<int>(255.999 * pixel_color.z()); // blue
//         }
//     }

//     SDL_UpdateTexture(sdl_image.texture, nullptr, pixels.data(), image.width * 3);

//     handle_image(sdl_image);

//     return 0;
// }

