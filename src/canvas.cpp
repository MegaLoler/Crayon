#include <canvas.h>
#include <cmath>
#include <iostream>

using namespace std;

double noise () {
    return (double) rand() / RAND_MAX;
}

Canvas::Canvas (int width, int height) : width (width), height (height) {
    background = new double[width * height];
    deposit = new double[width * height];
    deposit_ = new double[width * height];
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

double Canvas::get_height (Vec position, double *deposit) {
    if (deposit == nullptr)
        deposit = this->deposit;
    // TODO: maybe interpolate
    int x = floor (position.x);
    int y = floor (position.y);
    while (x < 0) x += width;
    while (y < 0) y += height;
    x %= width;
    y %= height;
    return deposit[x + y * width] + background[x + y * width];
}

double Canvas::get_wax (Vec position, double *deposit) {
    if (deposit == nullptr)
        deposit = this->deposit;
    // TODO: maybe interpolate
    int x = floor (position.x);
    int y = floor (position.y);
    while (x < 0) x += width;
    while (y < 0) y += height;
    x %= width;
    y %= height;
    return deposit[x + y * width];
}

void Canvas::deposit_wax (Vec position, double amount, double *deposit) {
    if (deposit == nullptr)
        deposit = this->deposit;
    // TODO: maybe interpolate
    int x = floor (position.x);
    int y = floor (position.y);
    while (x < 0) x += width;
    while (y < 0) y += height;
    x %= width;
    y %= height;
    deposit[x + y * width] += amount;
}

void Canvas::smear (Vec position, Vec velocity, Crayon crayon) {
    Vec half_crayon_size (crayon.width / 2, crayon.height / 2, 0);
    double flow_coefficient = 1 - (1 / (1 + velocity.distance ()));
    int x1 = position.x - half_crayon_size.x - 1;
    int y1 = position.y - half_crayon_size.y - 1;
    int x2 = x1 + crayon.width + 2;
    int y2 = y1 + crayon.height + 2;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int xx = x % width;
            int yy = y % height;
            while (xx < 0) xx += width;
            while (yy < 0) yy += height;
            deposit_[xx + yy * width] = deposit[xx + yy * width];
        }
    }
    for (int y = 0; y < crayon.height; y++) {
        for (int x = 0; x < crayon.width; x++) {
            Vec crayon_mask_position (x, y, 0);
            Vec canvas_position = position + crayon_mask_position - half_crayon_size;
            double wax_height = get_height (canvas_position);
            double crayon_height = crayon.mask[x + y * crayon.width];
            double smear[9];
            double total = 0;
            for (int sy = 0; sy < 3; sy++) {
                for (int sx = 0; sx < 3; sx++) {
                    int i = sx + sy * 3;
                    // relative smear mask coordinates from center
                    Vec smear_position (sx - 1, sy - 1, 0);
                    // corresponding canvas coordinates
                    Vec canvas_smear_position = canvas_position + smear_position;
                    if (!(smear_position.x == 0 && smear_position.y == 0)) {
                        double directional_smearing = velocity.dot (smear_position);
                        directional_smearing *= 1 - flow_coefficient;
                        double flow_smearing = wax_height - get_height (canvas_smear_position);
                        flow_smearing *= flow_coefficient;
                        double smearing = fmax (0, directional_smearing + flow_smearing);
                        smearing /= smear_position.distance ();
                        total += smearing;
                        smear[i] = smearing;
                    }
                }
            }
            if (total > 0) {
                for (int sy = 0; sy < 3; sy++) {
                    for (int sx = 0; sx < 3; sx++) {
                        int i = sx + sy * 3;
                        Vec smear_position (sx - 1, sy - 1, 0);
                        Vec canvas_smear_position = canvas_position + smear_position;
                        // do da smear
                        double smearing = smear[i];
                        smearing *= fmax (0, wax_height + (crayon_height - 1) * 2);
                        double wax = get_wax (canvas_position) * crayon.wax.viscosity * smearing / total / 8;
                        deposit_wax (canvas_position, -wax, deposit_);
                        deposit_wax (canvas_smear_position, wax, deposit_);
                    }
                }
            }
        }
    }
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int xx = x % width;
            int yy = y % height;
            while (xx < 0) xx += width;
            while (yy < 0) yy += height;
            deposit[xx + yy * width] = deposit_[xx + yy * width];
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
        smear (p, delta, crayon);
        //for (int y = 0; y < crayon.height; y++) {
        //    for (int x = 0; x < crayon.width; x++) {
        //        // this is all temp btw
        //        double crayon_height = crayon.mask[x + y * crayon.width];
        //        double ink = ink_step * crayon_height;
        //        int xx = p.x + x - crayon.width / 2;
        //        int yy = p.y + y - crayon.height / 2;
        //        int ix = floor (xx);
        //        int iy = floor (yy);
        //        //deposit[(ix % width) + (iy % height) * width] += ink;
        //        double rx = xx - ix;
        //        double ry = yy - iy;
        //        double irx = 1 - rx;
        //        double iry = 1 - ry;
        //        while (ix < 0) ix += width;
        //        while (iy < 0) iy += height;
        //        deposit[(ix%width)+(iy%height)*width]         += ink * irx * iry;
        //        deposit[((ix+1)%width)+(iy%height)*width]     += ink * rx  * iry;
        //        deposit[(ix%width)+((iy+1)%height)*width]     += ink * irx * ry;
        //        deposit[((ix+1)%width)+((iy+1)%height)*width] += ink * rx  * ry;
        //    }
        //}
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
