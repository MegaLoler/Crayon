#include <iostream>
#include <SFML/Graphics.hpp>
#include <canvas.h>

using namespace std;

const int default_width = 640;
const int default_height = 480;

int main (int argc, char **argv) {

    sf::RenderWindow window (sf::VideoMode (default_width, default_height), "Crayon");
    sf::CircleShape shape (100.f);
    shape.setFillColor (sf::Color::Green);

    while (window.isOpen ()) {
        sf::Event event;
        while (window.pollEvent (event)) {
            if (event.type == sf::Event::Closed)
                window.close ();
        }

        window.clear ();
        window.draw (shape);
        window.display ();
    }

    return 0;
}
