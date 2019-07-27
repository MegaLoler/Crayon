#include <vector>
#include <stroke.h>

using namespace std;

// a canvas that u can draw on
class Canvas {
    private:
        // collection of strokes made on the canvas to form the image
        vector<Stroke> strokes;

    public:
        Canvas () {}
        ~Canvas () {}
};
