#include <crayon.h>
#include <cmath>

Crayon::Crayon (int width, int height, Wax *wax) : width (width), height (height), wax (wax) {
    mask = new double[width * height];
    init_mask ();
}

Crayon::~Crayon () {
    //delete mask;
}

void Crayon::init_mask () {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double dx = 2.0 * x / width - 1;
            double dy = 2.0 * y / height - 1;
            double d = sqrt (dx * dx + dy * dy);
            double value = fmax (0, cos (d * M_PI / 2));
            mask[x + y * width] = fmin (1, value * 1000 - 500);
        }
    }
}
