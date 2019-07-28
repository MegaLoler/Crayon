#include <crayon.h>

Crayon::Crayon (int width, int height, Wax wax) : width (width), height (height), wax (wax) {
    mask = new double[width * height];
}

Crayon::~Crayon () {
    delete mask;
}
