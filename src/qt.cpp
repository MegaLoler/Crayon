#include <iostream>
#include <canvas.h>
#include <crayon.h>
#include <wax.h>
#include <vec.h>

#include <QApplication>
#include <QWidget>
#include <QPainter>

using namespace std;

const int default_width = 640;
const int default_height = 480;

class QImage_Canvas : public Canvas {
    private:
        QImage image;

    public:
        QImage_Canvas (int width, int height) : Canvas (width, height) {
            image = QImage (width, height, QImage::Format_RGB32);
        }

        QImage get_image () {
            return image;
        }

    protected:
        void draw_pixel (int x, int y, Vec color) {
            QRgb value = qRgb(color.x * 0xff, color.y * 0xff, color.z * 0xff);
            image.setPixel(x, y, value);
        }
};

class QT_Canvas : public QWidget {
    public:
        QImage_Canvas *canvas;

        QT_Canvas (QWidget *parent = 0) : QWidget (parent) {
            canvas = new QImage_Canvas (default_width, default_height);
        }

        ~QT_Canvas () {
            //delete canvas;
        }

    protected:
        void paintEvent (QPaintEvent *event) {
            Q_UNUSED (event);
            canvas->render ();
            QPainter painter (this);
            painter.drawImage (0, 0, canvas->get_image ());
        }
};

int main (int argc, char **argv) {

    // create the different colored waxes
    Wax *wax_red    = new Wax (0.95,  0.45, 0.45,  0.605,  0.051);
    Wax *wax_orange = new Wax (0.999, 0.55, 0.2,   0.605,  0.042);
    Wax *wax_yellow = new Wax (0.95,  0.9,  0.2,   0.715,  0.111);
    Wax *wax_green  = new Wax (0.35,  0.8,  0.35,  0.385,  0.096);
    Wax *wax_blue   = new Wax (0.3,   0.5,  0.9,   0.715,  0.054);
    Wax *wax_purple = new Wax (0.65,  0.4,  0.9,   0.385,  0.06);
    Wax *wax_brown  = new Wax (0.8,   0.55, 0.5,   0.3025, 0.15);
    Wax *wax_black  = new Wax (0.26,  0.25, 0.245, 0.11,   0.24);
    Wax *wax_grey   = new Wax (0.42,  0.4,  0.39,  0.44,   0.51);
    Wax *wax_white  = new Wax (0.8,   0.8,  0.79,  0.55,   0.33);

    // create the crayons with those waxes
    double diameter = 10;
    Crayon *crayon_red    = new Crayon (diameter, diameter, wax_red);
    Crayon *crayon_orange = new Crayon (diameter, diameter, wax_orange);
    Crayon *crayon_yellow = new Crayon (diameter, diameter, wax_yellow);
    Crayon *crayon_green  = new Crayon (diameter, diameter, wax_green);
    Crayon *crayon_blue   = new Crayon (diameter, diameter, wax_blue);
    Crayon *crayon_purple = new Crayon (diameter, diameter, wax_purple);
    Crayon *crayon_brown  = new Crayon (diameter, diameter, wax_brown);
    Crayon *crayon_black  = new Crayon (diameter, diameter, wax_black);
    Crayon *crayon_grey   = new Crayon (diameter, diameter, wax_grey);
    Crayon *crayon_white  = new Crayon (diameter, diameter, wax_white);

    // set the current crayon
    Crayon *crayon = crayon_red;

    //canvas.stroke (Vec (px, py), Vec (x, y), crayon, max_force * event.tfinger.pressure, smear);
    double px = 0;
    double py = 0;
    double x = 0;
    double y = 0;
    bool quit = false;
    bool smear = false;
    double max_force = 100;

    // the qt gui
    QApplication app (argc, argv);
    QT_Canvas window;

    window.resize (default_width, default_height);
    window.setWindowTitle ("Crayon");
    window.show ();

    return app.exec ();
}
