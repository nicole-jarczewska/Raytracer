#include <cmath>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>

//sdl
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sdl_handler.h"

//klasy etc.
#include "camera.h"
#include "color.h"
#include "material.h"
#include "ray.h"
#include "vec.h"
#include "objects.h"
#include "somewhere.h"
#include "hit.h"

