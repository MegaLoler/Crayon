#include <iostream>
#include <canvas.h>
#include <SDL2/SDL.h>

using namespace std;

const int default_width = 640;
const int default_height = 480;

int main (int argc, char **argv) {

    Canvas canvas (default_width, default_height);

    SDL_Window *window = nullptr;
    SDL_Surface *surface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "Could not initialize SDL: " << SDL_GetError ();
        exit (EXIT_FAILURE);
    }

    window = SDL_CreateWindow ("Crayon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, default_width, default_height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "Could not create SDL window: " << SDL_GetError ();
        exit (EXIT_FAILURE);
    }

    surface = SDL_GetWindowSurface (window);
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent (&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        SDL_FillRect (surface, nullptr, SDL_MapRGB (surface->format, 0xFF, 0xFF, 0xFF));
        SDL_UpdateWindowSurface (window);
    }

    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}
