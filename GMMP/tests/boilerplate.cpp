
#include <iostream>
#include <cassert>
#include <random>

#include <SFML/Graphics.hpp>
#include <sfml-3d/3d_camera.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>


int main() {
    const int HEIGHT = 1400;
    const int WIDTH = 2100;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Mental Modeller");

    std::cout << "window created" << std::endl;

    Camera camera(window, 60.0f, 0.001f, 2.f, 10.f, 0.5f);


    while (window.isOpen()) {
        while (const auto& event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
       

        // - - DRAWING - -
        window.clear();
        //camera.drawCrosshairIfNeeded(window);
        window.display();
    }

    return 0;
}