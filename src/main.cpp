#include <iostream>
#include <canvas.h>
#include <crayon.h>
#include <wax.h>
#include <vec.h>
#include <SDL2/SDL.h>

using namespace std;

const int default_width = 600;
const int default_height = 400;

int main (int argc, char **argv) {

    Canvas canvas (default_width, default_height);
    Wax *wax_red    = new Wax (0.95,  0.45, 0.45,  0.605,  0.051);
    Wax *wax_orange = new Wax (0.999, 0.55, 0.2,   0.605,  0.042);
    Wax *wax_yellow = new Wax (0.95,  0.9,  0.2,   0.715,  0.111);
    Wax *wax_green  = new Wax (0.35,  0.8,  0.35,  0.385,  0.096);
    Wax *wax_blue   = new Wax (0.3,   0.5,  0.9,   0.715,  0.054);
    Wax *wax_purple = new Wax (0.65,  0.4,  0.9,   0.385,  0.06);
    Wax *wax_brown  = new Wax (0.8,   0.55, 0.5,   0.3025, 0.15);
    Wax *wax_black  = new Wax (0.26,  0.25, 0.245, 0.11,   0.24);
    Wax *wax_grey   = new Wax (0.42,  0.4,  0.39,  0.44,   0.51);
    Wax *wax_white  = new Wax (0.8,   0.8,  0.79,  0.55,   0.33);
    Crayon *crayon_red    = new Crayon (10, 10, wax_red);
    Crayon *crayon_orange = new Crayon (10, 10, wax_orange);
    Crayon *crayon_yellow = new Crayon (10, 10, wax_yellow);
    Crayon *crayon_green  = new Crayon (10, 10, wax_green);
    Crayon *crayon_blue   = new Crayon (10, 10, wax_blue);
    Crayon *crayon_purple = new Crayon (10, 10, wax_purple);
    Crayon *crayon_brown  = new Crayon (10, 10, wax_brown);
    Crayon *crayon_black  = new Crayon (10, 10, wax_black);
    Crayon *crayon_grey   = new Crayon (10, 10, wax_grey);
    Crayon *crayon_white  = new Crayon (10, 10, wax_white);
    Crayon *crayon = crayon_red;

    // TEMP: for testing
    //for (int y = 100; y < 200; y++) {
    //    for (int x = 100; x < 200; x++) {
    //        Vec position (x, y, 0);
    //        canvas.deposit_wax (position, wax_orange, 1);
    //    }
    //}
    //for (int y = 150; y < 250; y++) {
    //    for (int x = 150; x < 250; x++) {
    //        Vec position (x, y, 0);
    //        canvas.deposit_wax (position, wax_red, 1);
    //    }
    //}

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

    double px = 0;
    double py = 0;
    double x = 0;
    double y = 0;
    bool quit = false;
    bool smear = false;
    double max_force = 100;
    SDL_Event event;
    while (!quit) {
        SDL_WaitEvent (&event);
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_FINGERDOWN:
                px = event.tfinger.x * default_width;
                py = event.tfinger.y * default_height;
                break;
            case SDL_FINGERMOTION:
                x = event.tfinger.x * default_width;
                y = event.tfinger.y * default_height;
                canvas.stroke (Vec (px, py), Vec (x, y), crayon, max_force * event.tfinger.pressure, smear);
                px = x;
                py = y;
                break;
            case SDL_MOUSEMOTION:
                x = event.motion.x;
                y = event.motion.y;
                if (event.button.button == SDL_BUTTON (SDL_BUTTON_LEFT)) {
                    canvas.stroke (Vec (px, py), Vec (x, y), crayon, max_force, smear);
                }
                px = x;
                py = y;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        crayon = crayon_red;
                        break;
                    case SDLK_2:
                        crayon = crayon_orange;
                        break;
                    case SDLK_3:
                        crayon = crayon_yellow;
                        break;
                    case SDLK_4:
                        crayon = crayon_green;
                        break;
                    case SDLK_5:
                        crayon = crayon_blue;
                        break;
                    case SDLK_6:
                        crayon = crayon_purple;
                        break;
                    case SDLK_7:
                        crayon = crayon_brown;
                        break;
                    case SDLK_8:
                        crayon = crayon_black;
                        break;
                    case SDLK_9:
                        crayon = crayon_grey;
                        break;
                    case SDLK_0:
                        crayon = crayon_white;
                        break;
                    case SDLK_c:
                        // clear canvas
                        canvas.clear_canvas ();
                        break;
                    case SDLK_t:
                        // reset crayon tip
                        crayon->init_mask ();
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
        canvas.render (renderer);

        // display texture
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, display, nullptr, nullptr);
        SDL_RenderPresent (renderer);
    }

    SDL_DestroyWindow (window);
    SDL_Quit ();

    return 0;
}
