// represents some wax material
struct Wax {
    double r;       // red color
    double g;       // green color
    double b;       // blue color
    double t;       // transmittance
    double s;       // scattering

    Wax (double r, double g, double b, double t, double s)
        : r (r), g (g), b (b), t (t), s (s) {};
};
