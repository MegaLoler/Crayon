#pragma once

#include <SDL2/SDL.h>
#include <wax.h>
#include <crayon.h>
#include <vec.h>

// stores ur drawing
class Canvas {
    private:
        double friction = 2; // frictional coefficient of canvas

        int width;
        int height;
        Wax wax = Wax (0.95, 0.45, 0.45, 0.605, 0.051); // tmp , the wax used in the deposit
        double *background; // the paper texture, a height map
        double *deposit;    // how much wax is deposited on the canvas at different spots
        double *deposit_;   // back buffer

        void adjust_height (Crayon &crayon, Vec position, double force);
        void smear (Vec position, Vec velocity, Crayon &crayon); // smear wax one step
        void draw_wax (Vec position, Vec velocity, Crayon &crayon, double force); // move wax from crayon to canvas due to friction

        double get_wax (Vec position, double *deposit = nullptr); // get amount of wax at position
        void deposit_wax (Vec position, double amount, double *deposit = nullptr); // deposit some wax here
        double get_height (Vec position, double *deposit = nullptr); // get the total height at a position
        double get_background_height (Vec position); // get height of the background

    public:
        Canvas (int width, int height);
        ~Canvas ();

        void generate_background ();    // generates the background txture
        void clear_canvas ();           // clears the wax deposit
        void render (SDL_Renderer *renderer, Crayon &crayon, int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
        void stroke (Vec p1, Vec p2, Crayon &crayon, double force); // draw a line with a crayon
};
