#pragma once

#include <vec.h>

// represents some wax material
struct Wax {
    double friction = 0.5; // frictional coefficient of wax
    double viscosity = 0.5; // viscosity
    double compression_resistance = 0.0005;

    Vec color;
    double t;       // transmittance
    double s;       // scattering

    Wax (double r, double g, double b, double t, double s)
        : color (Vec (r, g, b)), t (t), s (s) {};
    Wax (Vec color, double t, double s)
        : color (color), t (t), s (s) {};
};
