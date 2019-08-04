#pragma once

#include <wax.h>
#include <crayon.h>
#include <stack.h>
#include <vec.h>

// stroke mode
enum Mode {
    DRAW,       // draws wax
    SMEAR,      // smears wax
    ERASE,      // erases wax
};

// stores ur drawing
class Canvas {
    private:
        double friction = 2; // frictional coefficient of canvas

        Vec bg_color = Vec (1, 1, 1);

        int width;
        int height;
        double *background = nullptr; // the paper texture, a height map
        Stack *deposit = nullptr;     // how much wax is deposited on the canvas at different spots
        Stack *deposit_ = nullptr;    // back buffer

        // rectangle of area to redraw each frame
        Vec damage1;
        Vec damage2;
        bool damage;

        void erase (Vec position, Crayon *crayon, double force);
        void adjust_height (Crayon *crayon, Vec position, double force, bool scrape = false);
        void smear (Vec position, Vec velocity, Crayon *crayon); // smear wax one step
        void draw_wax (Vec position, Vec velocity, Crayon *crayon, double force); // move wax from crayon to canvas due to friction

        double get_wax (Vec position, Stack *deposit = nullptr); // get amount of wax at position
        void deposit_wax (Vec position, Wax *wax, double amount, Stack *deposit = nullptr); // deposit some wax here
        void deposit_stack (Vec position, Stack &stack, Stack *deposit = nullptr); // deposit a stack onto existing stack
        Stack take_wax (Vec position, double amount, Stack *deposit = nullptr); // take some wax from the stack here and get a stack of the result
        double get_height (Vec position, Stack *deposit = nullptr); // get the total height at a position
        double get_background_height (Vec position); // get height of the background

        Vec clamp_position (Vec position); // make sure a position is in bounds

        void free_resources ();

    protected:
        virtual void draw_pixel (int x, int y, Vec color) = 0;
        void resize_canvas (int width, int height);

    public:
        Canvas (int width, int height);
        virtual ~Canvas ();

        void set_bg_color (Vec color);
        void resize (int width, int height);
        void invalidate ();             // indicate entire screen region is damaged
        void generate_background ();    // generates the background txture
        void clear_canvas (Stack *deposit = nullptr, int width = 0, int height = 0);           // clears the wax deposit
        void render ();
        void stroke (void (*process) (void), Vec p1, Vec p2, Crayon *crayon, double force, Mode = DRAW); // draw a line with a crayon
};
