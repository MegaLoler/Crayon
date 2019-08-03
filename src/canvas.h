#pragma once

#include <SDL2/SDL.h>
#include <wax.h>
#include <crayon.h>
#include <stack.h>
#include <vec.h>

// stores ur drawing
class Canvas {
    //private:
    public:
        double friction = 2; // frictional coefficient of canvas

        int width;
        int height;
        double *background; // the paper texture, a height map
        Stack *deposit;     // how much wax is deposited on the canvas at different spots
        Stack *deposit_;    // back buffer

        // rectangle of area to redraw each frame
        Vec damage1;
        Vec damage2;
        bool damage;

        void adjust_height (Crayon *crayon, Vec position, double force, bool scrape = false);
        void smear (Vec position, Vec velocity, Crayon *crayon); // smear wax one step
        void draw_wax (Vec position, Vec velocity, Crayon *crayon, double force); // move wax from crayon to canvas due to friction

        double get_wax (Vec position, Stack *deposit = nullptr); // get amount of wax at position
        void deposit_wax (Vec position, Wax *wax, double amount, Stack *deposit = nullptr); // deposit some wax here
        void deposit_stack (Vec position, Stack &stack, Stack *deposit = nullptr); // deposit a stack onto existing stack
        Stack take_wax (Vec position, double amount, Stack *deposit = nullptr); // take some wax from the stack here and get a stack of the result
        double get_height (Vec position, Stack *deposit = nullptr); // get the total height at a position
        double get_background_height (Vec position); // get height of the background

    public:
        Canvas (int width, int height);
        ~Canvas ();

        void generate_background ();    // generates the background txture
        void clear_canvas ();           // clears the wax deposit
        void render (SDL_Renderer *renderer);
        void stroke (Vec p1, Vec p2, Crayon *crayon, double force, bool smear_only = false); // draw a line with a crayon
};
