#include <iostream>
#include <canvas.h>
#include <crayon.h>
#include <wax.h>
#include <vec.h>
#include <SDL2/SDL.h>

using namespace std;

const int default_width = 640;
const int default_height = 480;

int main (int argc, char **argv) {

    Canvas canvas (default_width, default_height);
    //Wax wax (0.95, 0.45, 0.45, 0.605, 0.051);
    Wax wax (0.999, 0.55, 0.2, 0.605, 0.042);
    //Wax wax (0.95, 0.9, 0.2, 0.715, 0.111);
    //Wax wax (0.35, 0.8, 0.35, 0.385, 0.096);
    //Wax wax (0.3, 0.5, 0.9, 0.715, 0.054);
    //Wax wax (0.65, 0.4, 0.9, 0.385, 0.06);
    Crayon crayon (15, 15, wax);

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

    SDL_Texture *display = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, default_width, default_height);

    bool quit = false;
    bool smear = false;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent (&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_MOUSEMOTION:
                if (event.button.button == SDL_BUTTON (SDL_BUTTON_LEFT)) {
                    double x = event.motion.x;
                    double y = event.motion.y;
                    double dx = event.motion.xrel;
                    double dy = event.motion.yrel;
                    double force = 100;
                    canvas.stroke (Vec (x - dx, y - dy), Vec (x, y), crayon, force, smear);
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_c:
                        // clear canvas
                        canvas.clear_canvas ();
                        break;
                    case SDLK_t:
                        // reset crayon tip
                        crayon.init_mask ();
                        break;
                    case SDLK_s:
                        //smear = true;
                        smear = !smear;
                        break;
                }
                break;
            //case SDL_KEYUP:
            //    switch (event.key.keysym.sym) {
            //        case SDLK_s:
            //            smear = false;
            //            break;
            //    }
            //    break;
        }

        // render to texture
        SDL_SetRenderTarget(renderer, display);
        canvas.render (renderer, crayon);

        // display texture
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, display, nullptr, nullptr);
        SDL_RenderPresent (renderer);
    }

    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}
