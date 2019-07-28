#include <wax.h>

// a crayon that u draw w/
class Crayon {
    private:
        int width;
        int height;
        double *mask;       // the height map of the crayon

        Wax wax;    // the wax its made of

    public:
        Crayon (int width, int height, Wax wax);
        ~Crayon ();
};
