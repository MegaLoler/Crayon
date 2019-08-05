#include <iostream>
#include <canvas.h>
#include <crayon.h>
#include <wax.h>
#include <vec.h>

#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
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

        void on_resize (int width, int height) {
            image = QImage (width, height, QImage::Format_RGB32);
            resize_canvas (width, height);
        }

    protected:
        void draw_pixel (int x, int y, Vec color) {
            QRgb value = qRgb(color.x * 0xff, color.y * 0xff, color.z * 0xff);
            image.setPixel(x, y, value);
        }
};

class QT_Canvas : public QWidget {
    private:
        bool mouse_down = false;
        double px = 0;
        double py = 0;
        double x = 0;
        double y = 0;
        Mode mode = DRAW;

        // the different colored waxes
        Wax *wax_red;
        Wax *wax_orange;
        Wax *wax_yellow;
        Wax *wax_green;
        Wax *wax_blue;
        Wax *wax_purple;
        Wax *wax_brown;
        Wax *wax_black;
        Wax *wax_grey;
        Wax *wax_white;

        // the crayon
        Crayon *crayon;

        // for constructing the title of the window
        string color_name;
        string size_name;

        static void process () {
            QCoreApplication::processEvents();
        }

        void set_title () {
            string mode_name;
            switch (mode) {
                case DRAW:
                    mode_name = "drawing";
                    break;
                case SMEAR:
                    mode_name = "smudging";
                    break;
                case ERASE:
                    mode_name = "erasing";
                    break;
                default:
                    mode_name = "?";
                    break;
            }
            string title = "Crayon [" + mode_name + " mode] [" + color_name + "] [" + size_name + "px]";
            setWindowTitle (QString::fromStdString (title));
        }

        void set_color (Wax *wax) {
            crayon->wax = wax;
            if (wax == wax_red)
                color_name = "red";
            else if (wax == wax_orange)
                color_name = "orange";
            else if (wax == wax_yellow)
                color_name = "yellow";
            else if (wax == wax_green)
                color_name = "green";
            else if (wax == wax_blue)
                color_name = "blue";
            else if (wax == wax_purple)
                color_name = "purple";
            else if (wax == wax_brown)
                color_name = "brown";
            else if (wax == wax_black)
                color_name = "black";
            else if (wax == wax_grey)
                color_name = "grey";
            else if (wax == wax_white)
                color_name = "white";
            set_title ();
        }

        void set_size (double diameter) {
            crayon->width = crayon->height = diameter;
            crayon->init_mask ();
            size_name = to_string ((int) diameter);
            set_title ();
        }

        void set_mode (Mode mode) {
            this->mode = mode;
            set_title ();
        }

    public:
        QImage_Canvas *canvas;

        QT_Canvas (int width, int height, QWidget *parent = 0) : QWidget (parent) {
            canvas = new QImage_Canvas (width, height);
            resize (width, height);
            setMouseTracking (true);
            setAttribute (Qt::WA_TabletTracking);

            // create the different colored waxes
            wax_red    = new Wax (0.95,  0.45, 0.45,  0.605,  0.051);
            wax_orange = new Wax (0.999, 0.55, 0.2,   0.605,  0.042);
            wax_yellow = new Wax (0.95,  0.9,  0.2,   0.715,  0.111);
            wax_green  = new Wax (0.35,  0.8,  0.35,  0.385,  0.096);
            wax_blue   = new Wax (0.3,   0.5,  0.9,   0.715,  0.054);
            wax_purple = new Wax (0.65,  0.4,  0.9,   0.385,  0.06);
            wax_brown  = new Wax (0.8,   0.55, 0.5,   0.3025, 0.15);
            wax_black  = new Wax (0.26,  0.25, 0.245, 0.11,   0.24);
            wax_grey   = new Wax (0.42,  0.4,  0.39,  0.44,   0.51);
            wax_white  = new Wax (0.8,   0.8,  0.79,  0.55,   0.33);

            // create the crayon
            crayon = new Crayon (10, 10, wax_red);

            // init color n size
            set_color (wax_red);
            set_size (10);
        }

        ~QT_Canvas () {
            delete canvas;
            delete wax_red;
            delete wax_orange;
            delete wax_yellow;
            delete wax_green;
            delete wax_blue;
            delete wax_purple;
            delete wax_brown;
            delete wax_black;
            delete wax_grey;
            delete wax_white;
            delete crayon;
        }

    protected:
        void paintEvent (QPaintEvent *event) {
            Q_UNUSED (event);
            canvas->render ();
            QPainter painter (this);
            painter.drawImage (0, 0, canvas->get_image ());
        }

        void keyPressEvent (QKeyEvent *event) {
            switch (event->key ()) {
                case Qt::Key_1:
                    set_color (wax_red);
                    break;
                case Qt::Key_2:
                    set_color (wax_orange);
                    break;
                case Qt::Key_3:
                    set_color (wax_yellow);
                    break;
                case Qt::Key_4:
                    set_color (wax_green);
                    break;
                case Qt::Key_5:
                    set_color (wax_blue);
                    break;
                case Qt::Key_6:
                    set_color (wax_purple);
                    break;
                case Qt::Key_7:
                    set_color (wax_brown);
                    break;
                case Qt::Key_8:
                    set_color (wax_black);
                    break;
                case Qt::Key_9:
                    set_color (wax_grey);
                    break;
                case Qt::Key_0:
                    set_color (wax_white);
                    break;
                case Qt::Key_C:
                    canvas->clear_canvas ();
                    update ();
                    break;
                case Qt::Key_B:
                    set_size (15);
                    break;
                case Qt::Key_M:
                    set_size (10);
                    break;
                case Qt::Key_L:
                    set_size (5);
                    break;
                case Qt::Key_D:
                    set_mode (DRAW);
                    break;
                case Qt::Key_S:
                    set_mode (SMEAR);
                    break;
                case Qt::Key_E:
                    set_mode (ERASE);
                    break;
                case Qt::Key_W:
                    canvas->set_bg_color (Vec (1, 1, 1));
                    update ();
                    break;
                case Qt::Key_I:
                    canvas->set_bg_color (Vec (0.26, 0.25, 0.245));
                    update ();
                    break;
                case Qt::Key_R:
                    canvas->set_bg_color (Vec (0.95, 0.45, 0.45));
                    update ();
                    break;
                case Qt::Key_G:
                    canvas->set_bg_color (Vec (0.35, 0.8, 0.35));
                    update ();
                    break;
                case Qt::Key_Q:
                    canvas->set_bg_color (Vec (0.3, 0.5, 0.9));
                    update ();
                    break;
                case Qt::Key_U:
                    canvas->undo ();
                    update ();
                    break;
                default:
                    break;
            }
        }

        void tabletEvent (QTabletEvent *event) {
            x = event->x ();
            y = event->y ();
            int ppx = px;
            int ppy = py;
            px = x;
            py = y;
            double pressure = event->pressure ();
            switch (event->type ()) {
                case QEvent::TabletMove:
                    if (mouse_down) {
                        canvas->stroke (process, Vec (ppx, ppy), Vec (x, y), crayon, pressure, mode);
                        update ();
                    }
                    break;
                case QEvent::TabletPress:
                    mouse_down = true;
                    canvas->save_state ();
                    break;
                case QEvent::TabletRelease:
                    mouse_down = false;
                    break;
                default:
                    break;
            }
        }

        void mouseMoveEvent (QMouseEvent *event) {
            x = event->x ();
            y = event->y ();
            int ppx = px;
            int ppy = py;
            px = x;
            py = y;
            if (mouse_down) {
                canvas->stroke (process, Vec (ppx, ppy), Vec (x, y), crayon, 0.5, mode);
                update ();
            }
        }

        void mousePressEvent (QMouseEvent *event) {
            mouse_down = true;
            canvas->save_state ();
        }

        void mouseReleaseEvent (QMouseEvent *event) {
            mouse_down = false;
        }

        void resizeEvent (QResizeEvent *event) {
            QSize size = event->size ();
            int width = size.width ();
            int height = size.height ();
            canvas->on_resize (width, height);
            update ();
        }
};

int main (int argc, char **argv) {

    QApplication app (argc, argv);
    QT_Canvas window (default_width, default_height);
    window.show ();

    return app.exec ();
}
