#include <vector>
#include <crayon.h>
#include <stroke_point.h>

using namespace std;

// a stroke that u draw on a canvas w a crayon
class Stroke {
    private:
        // the crayon that was used to draw this
        Crayon crayon;

        // sequence of points outlining the shape of the stroke
        vector<StrokePoint> points;

    public:
        Stroke () {}
        ~Stroke () {}
};
