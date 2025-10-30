#pragma once

struct IMAGE{
    int width;
    int height;
    double aspect_ratio;

    IMAGE(int w, double ar)
        : aspect_ratio(ar), width(w),
          height(int(w / ar))
    {
        if (height < 1) height = 1;
    }
};

struct SDL_IMAGE{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

SDL_IMAGE initiate_image(IMAGE image){
    SDL_IMAGE sdl_image;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
    }

    SDL_Window* window = SDL_CreateWindow("Raytracer - Latarnia Morska",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        image.width, image.height, 0);

    if (!window) {
        std::cerr << "SDL CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        image.width, image.height);
    
    sdl_image.window = window;
    sdl_image.renderer = renderer;
    sdl_image.texture = texture;

    return sdl_image;
}

void handle_image(SDL_IMAGE sdl_image){
    SDL_RenderClear(sdl_image.renderer);
    SDL_RenderCopy(sdl_image.renderer, sdl_image.texture, nullptr, nullptr);
    SDL_RenderPresent(sdl_image.renderer);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }
    }

    SDL_DestroyTexture(sdl_image.texture);
    SDL_DestroyRenderer(sdl_image.renderer);
    SDL_DestroyWindow(sdl_image.window);
    SDL_Quit();
}