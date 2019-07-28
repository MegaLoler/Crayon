#include <canvas.h>
#include <cmath>

using namespace std;

double noise () {
    return (double) rand() / RAND_MAX;
}

Canvas::Canvas (int width, int height) : width (width), height (height) {
    background = new double[width * height];
    deposit = new double[width * height];
    generate_background ();
    clear_canvas ();
}

Canvas::~Canvas () {
    //delete background;
    //delete deposit;
}

void Canvas::render (SDL_Renderer *renderer, int x1, int y1, int x2, int y2) {
    x2 = x2 ? x2 : width;
    y2 = y2 ? y2 : height;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            double value = background[x + y * width];
            Vec bg_color (value, value, value);
            Vec wax_color = wax.color;
            Vec unit (1, 1, 1);
            double wax_thickness = deposit[x + y * width];
            Vec transmittance = (wax_color * wax.t).pow (wax_thickness) * bg_color;
            Vec reflectance = unit - (unit - wax_color).pow (wax_thickness * wax.s);
            Vec color = reflectance + transmittance;
            SDL_SetRenderDrawColor (renderer, color.x * 0xff, color.y * 0xff, color.z * 0xff, 0xff);
            SDL_RenderDrawPoint (renderer, x, y);
        }
    }
}

void Canvas::stroke (Vec p1, Vec p2, Crayon crayon) {
    // iterate across the line
    Vec delta = p2 - p1;
    double distance = delta.distance ();
    int iterations = ceil (distance);
    double total_ink = distance * .1;    // total amount in stroke
    double ink_step = total_ink / iterations; // amount deposited in one iterations
    Vec step = delta / iterations;
    Vec p = p1;
    for (int i = 0; i < iterations; i++) {
        for (int y = 0; y < crayon.height; y++) {
            for (int x = 0; x < crayon.width; x++) {
                double ink = ink_step * crayon.mask[x + y * crayon.width];
                int xx = p.x + x - crayon.width / 2;
                int yy = p.y + y - crayon.height / 2;
                int ix = floor (xx);
                int iy = floor (yy);
                //deposit[(ix % width) + (iy % height) * width] += ink;
                double rx = xx - ix;
                double ry = yy - iy;
                double irx = 1 - rx;
                double iry = 1 - ry;
                while (ix < 0) ix += width;
                while (iy < 0) iy += height;
                deposit[(ix%width)+(iy%height)*width]         += ink * irx * iry;
                deposit[((ix+1)%width)+(iy%height)*width]     += ink * rx  * iry;
                deposit[(ix%width)+((iy+1)%height)*width]     += ink * irx * ry;
                deposit[((ix+1)%width)+((iy+1)%height)*width] += ink * rx  * ry;
            }
        }
        p = p + step;
    }
}

void Canvas::clear_canvas () {
    for (int i = 0; i < width * height; i++)
        deposit[i] = 0;

    // temp for testing
    for (int y = 100; y < 200; y++) {
        for (int x = 100; x < 200; x++) {
            deposit[x + y * width] = 1;
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
    for (int i = 0; i < width * height; i++) {
        background[i] = (0xff - background[i]) / 0xff;
    }
}
