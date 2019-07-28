#pragma once

#include <wax.h>

// a crayon that u draw w/
struct Crayon {
    int width;
    int height;
    double *mask;       // the height map of the crayon

    Wax wax;    // the wax its made of

    Crayon (int width, int height, Wax wax);
    ~Crayon ();

    void init_mask ();  // initialize the crayon's mask
};
