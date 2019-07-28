#pragma once

#include <SDL2/SDL.h>
#include <wax.h>
#include <crayon.h>
#include <vec.h>

// stores ur drawing
class Canvas {
    private:
        int width;
        int height;
        Wax wax = Wax (0.95, 0.45, 0.45, 0.605, 0.051); // tmp , the wax used in the deposit
        double *background; // the paper texture, a height map
        double *deposit;    // how much wax is deposited on the canvas at different spots

    public:
        Canvas (int width, int height);
        ~Canvas ();

        void generate_background ();    // generates the background txture
        void clear_canvas ();           // clears the wax deposit
        void render (SDL_Renderer *renderer, int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
        void stroke (Vec p1, Vec p2, Crayon crayon); // draw a line with a crayon
};
