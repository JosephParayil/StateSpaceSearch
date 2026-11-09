
#include <iostream>
#include <cassert>
#include <random>

#include <SFML/Graphics.hpp>
#include <sfml-3d/3d_camera.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>

#include "GraphMM.hpp"
#include "MM_3D.hpp"
#include "MM_Sandbox.hpp"

int main() {
    const int HEIGHT = 1400;
    const int WIDTH = 2100;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Mental Modeller");

    std::cout << "window created" << std::endl;

    Camera camera(window, 60.0f, 0.001f, 2.f, 10.f, 0.5f);  


    MM_Sandbox my3DMM;
    my3DMM.addNode("Sappa");
    my3DMM.addNode("Papa");
    my3DMM.addNode("Sappa Papa");
    my3DMM.addConnection("Sappa Papa", "Sappa");
    my3DMM.addConnection("Sappa Papa", "Papa");
    my3DMM.update3DObjects();
    std::cout << "mm created" << std::endl;




    while (window.isOpen()) {
        while (const auto& event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else {
                camera.handleEvent(event);
                my3DMM.handleEvent(window, event);
            }
            
        }
        // - - UPDATES - -
        my3DMM.updateSelectionOnPoint(window, camera, sf::Vector2f(sf::Mouse::getPosition(window)));
        /*
        sf::FloatRect select_bounds(
            sf::Vector2f(200.f, 300.f),
            sf::Vector2f(500.f, 300.f)
        );
        my3DMM.updateSelectionOnRect(window, camera, select_bounds);
        */
        camera.update();
        my3DMM.computePhysics();


        // - - DRAWING - -
        window.clear();
        my3DMM.render(window, camera);
        camera.drawCrosshairIfNeeded(window);
        window.display();
    }

    return 0;
}