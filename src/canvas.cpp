#include <canvas.h>
#include <cmath>

double noise () {
    return (double) rand() / RAND_MAX;
}

Canvas::Canvas (int width, int height) : width (width), height (height) {
    background = new double[width * height];
    wax = new double[width * height];
    generate_background ();
}

Canvas::~Canvas () {
    delete background;
    delete wax;
}

void Canvas::render (SDL_Renderer *renderer, int x1, int y1, int x2, int y2) {
    x2 = x2 ? x2 : width;
    y2 = y2 ? y2 : height;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            SDL_SetRenderDrawColor (renderer, 0xff, 0x00, 0x00, 0x00);
            SDL_RenderDrawPoint (renderer, x, y);
        }
    }
}

void Canvas::generate_background () {
    const int fiber_count = 100000;
    const int fiber_length = 100;
    const double acceleration = 0.2;
    const double velocity = 1;
    const double ink = 1;
    for (int i = 0; i < fiber_count; i++) {
        double x = noise () * width;
        double y = noise () * height;
        double xv = noise () * velocity;
        double yv = noise () * velocity;
        for (int j = 0; j < fiber_length; j++) {
            int ix = floor (x);
            int iy = floor (y);
            double rx = x - ix;
            double ry = y - iy;
            double irx = 1 - rx;
            double iry = 1 - ry;
            background[(ix%width)+(iy%height)*width]         += ink * irx * iry;
            background[((ix+1)%width)+(iy%height)*width]     += ink * rx  * iry;
            background[(ix%width)+((iy+1)%height)*width]     += ink * irx * ry;
            background[((ix+1)%width)+((iy+1)%height)*width] += ink * rx  * ry;
            x += xv;
            y += yv;
            xv += (noise () - 0.5) * acceleration;
            yv += (noise () - 0.5) * acceleration;
        }
    }
}
