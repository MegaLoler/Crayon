#include <iostream>
#include <canvas.h>
#include <crayon.h>
#include <wax.h>
#include <vec.h>

#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QThread>
#include <QObject>

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

class Worker : public QObject {
    Q_OBJECT

    private:
        QT_Canvas *window;

    public:
        Worker (QT_Canvas *window) : window (window) {}

    public slots:
        void process() {
            Vec p1 = Vec (window->px, window->py);
            Vec p2 = Vec (window->x, window->y);
            window->canvas->stroke (p1, p2, window->crayon, window->max_force, window->smear);
        }

    signals:
        void finished () {
            window->update ();
        }

        void error (QString err) {
            // print it or somethin idk
        }
};

class QT_Canvas : public QWidget {
    private:
        bool mouse_down = false;
        double px = 0;
        double py = 0;
        double x = 0;
        double y = 0;
        bool smear = false;
        double max_force = 100;

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

        // the crayons with those waxes
        Crayon *crayon_red;
        Crayon *crayon_orange;
        Crayon *crayon_yellow;
        Crayon *crayon_green;
        Crayon *crayon_blue;
        Crayon *crayon_purple;
        Crayon *crayon_brown;
        Crayon *crayon_black;
        Crayon *crayon_grey;
        Crayon *crayon_white;

        // the current crayon
        Crayon *crayon;

    public:
        QImage_Canvas *canvas;

        QT_Canvas (int width, int height, QWidget *parent = 0) : QWidget (parent) {
            canvas = new QImage_Canvas (width, height);
            resize (width, height);
            setWindowTitle ("Crayon");
            setMouseTracking (true);

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

            // create the crayons with those waxes
            double diameter = 10;
            crayon_red    = new Crayon (diameter, diameter, wax_red);
            crayon_orange = new Crayon (diameter, diameter, wax_orange);
            crayon_yellow = new Crayon (diameter, diameter, wax_yellow);
            crayon_green  = new Crayon (diameter, diameter, wax_green);
            crayon_blue   = new Crayon (diameter, diameter, wax_blue);
            crayon_purple = new Crayon (diameter, diameter, wax_purple);
            crayon_brown  = new Crayon (diameter, diameter, wax_brown);
            crayon_black  = new Crayon (diameter, diameter, wax_black);
            crayon_grey   = new Crayon (diameter, diameter, wax_grey);
            crayon_white  = new Crayon (diameter, diameter, wax_white);

            // set the current crayon
            crayon = crayon_red;
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
            delete crayon_red;
            delete crayon_orange;
            delete crayon_yellow;
            delete crayon_green;
            delete crayon_blue;
            delete crayon_purple;
            delete crayon_brown;
            delete crayon_black;
            delete crayon_grey;
            delete crayon_white;
        }

    protected:
        void paintEvent (QPaintEvent *event) {
            Q_UNUSED (event);
            canvas->render ();
            QPainter painter (this);
            painter.drawImage (0, 0, canvas->get_image ());
        }

        void mouseMoveEvent (QMouseEvent *event) {
            x = event->x ();
            y = event->y ();
            if (mouse_down) {
                QThread* thread = new QThread;
                Worker* worker = new Worker (this);
                worker->moveToThread (thread);
                connect(worker, SIGNAL (error(QString)), this, SLOT (errorString(QString)));
                connect(thread, SIGNAL (started()), worker, SLOT (process()));
                connect(worker, SIGNAL (finished()), thread, SLOT (quit()));
                connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
                connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
                thread->start();
            }
            px = x;
            py = y;
        }

        void mousePressEvent (QMouseEvent *event) {
            mouse_down = true;
        }

        void mouseReleaseEvent (QMouseEvent *event) {
            mouse_down = false;
        }

    friend class Worker;
};

int main (int argc, char **argv) {

    QApplication app (argc, argv);
    QT_Canvas window (default_width, default_height);
    window.show ();

    return app.exec ();
}