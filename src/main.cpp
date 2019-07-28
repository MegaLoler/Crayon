#include <iostream>
#include <canvas.h>
#include <SDL2/SDL.h>

using namespace std;

const int default_width = 640;
const int default_height = 480;

int main (int argc, char **argv) {

    Canvas canvas (default_width, default_height);

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "Could not initialize SDL: " << SDL_GetError ();
        exit (EXIT_FAILURE);
    }

    SDL_CreateWindowAndRenderer(default_width, default_height, 0, &window, &renderer);
    if (window == nullptr) {
        cerr << "Could not create SDL window: " << SDL_GetError ();
        exit (EXIT_FAILURE);
    }

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent (&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        canvas.render (renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}
