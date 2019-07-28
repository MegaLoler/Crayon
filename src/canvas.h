#include <SDL2/SDL.h>

// stores ur drawing
class Canvas {
    private:
        int width;
        int height;
        double *background; // the paper texture, a height map
        double *wax;        // how much wax is deposited on the canvas at different spots

    public:
        Canvas (int width, int height);
        ~Canvas ();

        void generate_background (); // generates the background txture
        void render (SDL_Renderer *renderer, int x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0);
};
