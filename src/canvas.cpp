#include <canvas.h>
#include <cmath>
#include <iostream>

using namespace std;

double noise () {
    return (double) rand() / RAND_MAX;
}

Canvas::Canvas (int width, int height) : width (width), height (height) {
    resize_canvas (width, height);
    generate_background ();
}

Canvas::~Canvas () {
    free_resources ();
}

void Canvas::free_resources () {
    if (background != nullptr)
        delete background;

    //for (int i = 0; i < width * height; i++) {
    //    if (deposit != nullptr)
    //        delete deposit[i];
    //    if (deposit_ != nullptr)
    //        delete deposit_[i];
    //}

    if (deposit != nullptr)
        delete deposit;
    if (deposit_ != nullptr)
        delete deposit_;
    if (undo_deposit != nullptr)
        delete undo_deposit;
}

void Canvas::save_state () {
    for (int i = 0; i < width * height; i++)
        undo_deposit[i] = deposit[i];
}

void Canvas::undo () {
    Stack *temp = deposit;
    deposit = undo_deposit;
    undo_deposit = temp;
    invalidate ();
}

void Canvas::resize_canvas (int width, int height) {
    int size = width * height;
    double *new_background = new double[size];
    Stack *new_deposit = new Stack[size];
    Stack *new_deposit_ = new Stack[size];
    Stack *new_undo_deposit = new Stack[size];
    
    for (int i = 0; i < size; i++) {
        new_background[i] = 0;
    }

    clear_canvas (new_deposit, width, height);
    if (deposit != nullptr) {
        int min_width = fmin (width, this->width);
        int min_height = fmin (height, this->height);
        for (int y = 0; y < min_height; y++) {
            for (int x = 0; x < min_width; x++) {
                new_deposit[x + y * width] = deposit[x + y * this->width];
            }
        }
    }
    // reuse the background, just wrap it
    // bc its too slow to regen each time lol
    if (background != nullptr) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int bg_x = x % this->width;
                int bg_y = y % this->height;
                new_background[x + y * width] = background[bg_x + bg_y * this->width];
            }
        }
    }

    free_resources ();

    this->width = width;
    this->height = height;
    background = new_background;
    deposit = new_deposit;
    deposit_ = new_deposit_;
    undo_deposit = new_undo_deposit;
    invalidate ();
}

void Canvas::set_bg_color (Vec color) {
    bg_color = color;
    invalidate ();
}

void Canvas::render () {
    int x1 = fmax (0, fmin (width, damage1.x));
    int y1 = fmax (0, fmin (height, damage1.y));
    int x2 = fmax (0, fmin (width, damage2.x));
    int y2 = fmax (0, fmin (height, damage2.y));
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int i = x + y * width;
            double value = background[i];
            Vec bg = bg_color * value;
            Vec color = deposit[i].render (bg);
            draw_pixel (x, y, color);
        }
    }
//    // temp: draw the crayon mask in the corner
//    for (int y = 0; y < crayon->height; y++) {
//        for (int x = 0; x < crayon->width; x++) {
//            double value = crayon->mask[x + y * crayon->width];
//            value = fmax (0, fmin (1, value)) * 0xff;
//            SDL_SetRenderDrawColor (renderer, value, value, value, 0xff);
//            SDL_RenderDrawPoint (renderer, x, y);
//        }
//    }
    damage1 = Vec (0, 0, 0);
    damage2 = Vec (0, 0, 0);
    damage = false;
}

double Canvas::get_height (Vec position, Stack *deposit) {
    return get_background_height (position) + get_wax (position, deposit);
}

Vec Canvas::clamp_position (Vec position) {
    // TODO: maybe interpolate, maybe wrap instead
    return Vec (fmax (0, fmin (width - 1, position.x)), fmax (0, fmin (height - 1, position.y)), 0);
}

double Canvas::get_background_height (Vec position) {
    position = clamp_position (position);
    int x = position.x;
    int y = position.y;
    // TODO: consider this
    //return pow (background[x + y * width], 10) * 2;
    return background[x + y * width];
}

// returns net thickness
double Canvas::get_wax (Vec position, Stack *deposit) {
    position = clamp_position (position);
    if (deposit == nullptr)
        deposit = this->deposit;
    int x = position.x;
    int y = position.y;
    return deposit[x + y * width].thickness ();
}

void Canvas::deposit_wax (Vec position, Wax *wax, double amount, Stack *deposit) {
    position = clamp_position (position);
    if (deposit == nullptr)
        deposit = this->deposit;
    int x = position.x;
    int y = position.y;
    deposit[x + y * width].deposit (wax, amount);
}

void Canvas::deposit_stack (Vec position, Stack &stack, Stack *deposit) {
    position = clamp_position (position);
    if (deposit == nullptr)
        deposit = this->deposit;
    int x = position.x;
    int y = position.y;
    deposit[x + y * width].deposit_stack (stack);
}

Stack Canvas::take_wax (Vec position, double amount, Stack *deposit) {
    position = clamp_position (position);
    if (deposit == nullptr)
        deposit = this->deposit;
    int x = position.x;
    int y = position.y;
    return deposit[x + y * width].take (amount);
}

void Canvas::smear (Vec position, Vec velocity, Crayon *crayon) {
    Vec half_crayon_size (crayon->width / 2, crayon->height / 2, 0);
    double flow_coefficient = 1 - (1 / (1 + velocity.distance ()));
    int x1 = position.x - half_crayon_size.x - 1;
    int y1 = position.y - half_crayon_size.y - 1;
    int x2 = x1 + crayon->width + 2;
    int y2 = y1 + crayon->height + 2;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int xx = x % width;
            int yy = y % height;
            while (xx < 0) xx += width;
            while (yy < 0) yy += height;
            deposit_[xx + yy * width] = deposit[xx + yy * width];
        }
    }
    for (int y = 0; y < crayon->height; y++) {
        for (int x = 0; x < crayon->width; x++) {
            Vec crayon_mask_position (x, y, 0);
            Vec canvas_position = position + crayon_mask_position - half_crayon_size;
            double wax_height = get_height (canvas_position);
            double crayon_height = crayon->mask[x + y * crayon->width];
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
                        smearing *= fmax (0, wax_height + crayon_height - 1);
                        // TODO: get viscosity of target wax instead
                        double wax = get_wax (canvas_position) * crayon->wax->viscosity * smearing / total / 8;
                        Stack taken = take_wax (canvas_position, wax, deposit_);
                        deposit_stack (canvas_smear_position, taken, deposit_);
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

void Canvas::adjust_height (Crayon *crayon, Vec position, double force, bool scrape) {
    // get highest part of crayon mask
    double crayon_max = 0;
    for (int i = 0; i < crayon->width * crayon->height; i++) {
        if (crayon->mask[i] > crayon_max)
            crayon_max = crayon->mask[i];
    }
    // lower the crayon to base level (= 1)
    double delta = 1 - crayon_max;
    for (int i = 0; i < crayon->width * crayon->height; i++)
        crayon->mask[i] += delta;

    if (scrape)
        return;

    double wax_resist = crayon->wax->compression_resistance;
    double epsilon = wax_resist / 4;

    // find out the min and max paper cell heights
    double paper_min = 1000;
    double paper_max = 0;
    Vec half_crayon_size (crayon->width / 2, crayon->height / 2, 0);
    Vec origin = position - half_crayon_size;
    int x1 = position.x - half_crayon_size.x;
    int y1 = position.y - half_crayon_size.y;
    int x2 = x1 + crayon->width;
    int y2 = y1 + crayon->height;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            //double height = get_background_height (Vec (x, y, 0));
            double height = get_height (Vec (x, y, 0));
            if (height < paper_min)
                paper_min = height;
            if (height > paper_max)
                paper_max = height;
        }
    }
    // figure out how far down the given force is able to push teh crayon
    while ((paper_max - paper_min) > epsilon) {
        double mid = (paper_max + paper_min) / 2;
        double required_force = 0;
        // figure out the required force
        int i = 0;
        for (int y = 0; y < crayon->height; y++) {
            for (int x = 0; x < crayon->width; x++) {
                Vec paper_position = origin + Vec (x, y, 0);
                double paper_height = get_height (paper_position);
                double crayon_height = crayon->mask[i++] - mid;
                double delta = paper_height + crayon_height - 1;
                if (delta > 0)
                    required_force += wax_resist * delta;
            }
        }
        /////////
        if (force < required_force)
            paper_min = mid;
        else
            paper_max = mid;
    }
    /////////

    // move the crayon up to that place
    double mid = (paper_max + paper_min) / 2;
    for (int i = 0; i < crayon->width * crayon->height; i++)
        crayon->mask[i] -= mid;
}

void Canvas::draw_wax (Vec position, Vec velocity, Crayon *crayon, double force) {
    Vec half_crayon_size (crayon->width / 2, crayon->height / 2, 0);
    int x1 = position.x - half_crayon_size.x - 1;
    int y1 = position.y - half_crayon_size.y - 1;
    int x2 = x1 + crayon->width + 2;
    int y2 = y1 + crayon->height + 2;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int xx = x % width;
            int yy = y % height;
            while (xx < 0) xx += width;
            while (yy < 0) yy += height;
            deposit_[xx + yy * width] = deposit[xx + yy * width];
        }
    }
    for (int y = 0; y < crayon->height; y++) {
        for (int x = 0; x < crayon->width; x++) {
            int crayon_i = x + y * crayon->width;
            Vec crayon_mask_position (x, y, 0);
            Vec canvas_position = position + crayon_mask_position - half_crayon_size;
            double crayon_height = crayon->mask[crayon_i];
            // get the paper cell in the movement direction
            double distance = fmax (0.01, fmax (abs (velocity.x), abs (velocity.y)));
            Vec dir = velocity / distance;
            Vec adjacent_position = canvas_position + dir;
            double speed = velocity.distance ();
            //double wax_height = get_background_height (canvas_position);
            //double adjacent = get_background_height (adjacent_position);
            double wax_height = get_height (canvas_position);
            double adjacent = get_height (adjacent_position);
            double slope = (adjacent - wax_height) / distance;
            double angle = atan (slope);
            Vec normal = Vec (cos (angle), -sin (angle), 0);
            double dp = normal.dot (Vec (0, -1, 0));
            dp = fmax (0, dp);
            double blend = 1 / (1 + get_wax (adjacent_position));
            double paper_friction = blend * friction;
            double wax_friction = (1 - blend) * crayon->wax->friction;
            double total_friction = paper_friction + wax_friction;
            double amount = (total_friction * dp) * force * speed;
            amount *= fmax (0, adjacent + crayon_height - 1);
            amount = fmax (0, fmin (0.1, amount));
            crayon->mask[crayon_i] -= amount;
            deposit_wax (canvas_position, crayon->wax, amount, deposit_);
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

void Canvas::erase (Vec position, Crayon *crayon, double force) {
    Vec half_crayon_size (crayon->width / 2, crayon->height / 2, 0);
    int x1 = position.x - half_crayon_size.x;
    int y1 = position.y - half_crayon_size.y;
    int x2 = x1 + crayon->width;
    int y2 = y1 + crayon->height;
    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
            int xx = x % width;
            int yy = y % height;
            while (xx < 0) xx += width;
            while (yy < 0) yy += height;
            Stack *stack = &deposit[xx + yy * width];
            stack->take (stack->thickness () * force);
        }
    }
}

void Canvas::stroke (void (*process) (void), Vec p1, Vec p2, Crayon *crayon, double force, Mode mode) {
    // iterate across the line
    Vec delta = p2 - p1;
    double distance = delta.distance ();
    int iterations = ceil (distance);
    Vec step = delta / iterations;
    Vec p = p1;
    for (int i = 0; i < iterations; i++) {
        adjust_height (crayon, p, force * 100, mode != DRAW);
        if (mode == ERASE) {
            erase (p, crayon, force);
            smear (p, delta * 10000, crayon);
        } else if (mode == DRAW) {
            smear (p, delta, crayon);
            draw_wax (p, delta, crayon, force * 100);
        } else if (mode == SMEAR) {
            smear (p, delta * 10000, crayon);
        }
        p = p + step;
        // call the process callback intermittantly
        process ();
    }
    Vec half (crayon->width / 2 + 2, crayon->height / 2 + 2, 0);
    Vec new_damage1 = Vec (fmin (p1.x, p2.x), fmin (p1.y, p2.y)) - half;
    Vec new_damage2 = Vec (fmax (p1.x, p2.x), fmax (p1.y, p2.y)) + half;
    if (damage) {
        damage1 = Vec (fmin (new_damage1.x, damage1.x), fmin (new_damage1.y, damage1.y));
        damage2 = Vec (fmax (new_damage2.x, damage2.x), fmax (new_damage2.y, damage2.y));
    } else {
        damage1 = new_damage1;
        damage2 = new_damage2;
    }
    damage = true;
}

void Canvas::clear_canvas (Stack *deposit, int width, int height) {
    if (deposit == nullptr) {
        deposit = this->deposit;
        width = this->width;
        height = this->height;
    }
    for (int i = 0; i < width * height; i++)
        deposit[i].clear ();
    invalidate ();
}

void Canvas::invalidate () {
    damage1 = Vec (0, 0, 0);
    damage2 = Vec (width, height, 0);
    damage = true;
}

void Canvas::generate_background () {
    const int fiber_count = width * height / 3.072;
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
            while (ix < 0) ix += width;
            while (iy < 0) iy += height;
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
