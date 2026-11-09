#include <iostream>
#include <cassert>
#include <random>

#include <SFML/Graphics.hpp>
#include <sfml-3d/3d_camera.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>

#include "GraphMM.hpp"
#include "MM_3D.hpp"
#include "GMMP.hpp"

int main() {
    const int HEIGHT = 1400;
    const int WIDTH = 2100;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Mental Modeller");
    std::cout << "window created" << std::endl;
    Camera camera(window, 60.0f, 0.001f, 2.f, 10.f, 0.5f);  
    
    tgui::Gui gui(window);
    GMMP gmmp(gui);
    
    std::cout << "mm created" << std::endl;
    std::cout << std::endl;
    
    gmmp.addNode("Sappa");
    gmmp.addNode("Sappa Papa");

    gmmp.nodes["Sappa Papa"]->body = "Father of [[Sappa]]";
    gmmp.resolveNodeConnections("Sappa Papa");
    gmmp.print();
    std::cout << std::endl;
    
    gmmp.nodes["Sappa Papa"]->body = "Father of [[No One]]";
    gmmp.resolveNodeConnections("Sappa Papa");
    gmmp.print();
    std::cout << std::endl;

    gmmp.nodes["Sappa Papa"]->body = "Father of [[Sappa]]";
    gmmp.resolveNodeConnections("Sappa Papa");
    gmmp.print();
    std::cout << std::endl;


    gmmp.nodes["Sappa"]->body = "Son of the [[sappa papa]]!";
    gmmp.resolveNodeConnections("Sappa");
    gmmp.print();
    std::cout << std::endl;

    gmmp.nodes["Sappa Papa"]->body = "";
    gmmp.resolveNodeConnections("Sappa Papa");
    gmmp.print();
    std::cout << std::endl;

    gmmp.nodes["Sappa Papa"]->body = "Was [[somebody]]";
    gmmp.resolveNodeConnections("Sappa Papa");
    gmmp.print();
    std::cout << std::endl;

    gmmp.nodes["Sappa"]->body = "Son of [[somebody]]!";
    gmmp.resolveNodeConnections("Sappa");
    gmmp.print();
    std::cout << std::endl;

    gmmp.nodes["Sappa Papa"]->body = "null";
    gmmp.resolveNodeConnections("Sappa Papa");
    gmmp.nodes["Sappa"]->body = "null";
    gmmp.resolveNodeConnections("Sappa");
    gmmp.print();
    std::cout << std::endl;


    while (window.isOpen()) {
        while (const auto& event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else {
                camera.handleEvent(event);
                gmmp.handleEvent(window, event, camera);
            }
            
        }
        // - - UPDATES - -
        gmmp.updateSelectionOnPoint(window, camera, sf::Vector2f(sf::Mouse::getPosition(window)));
        camera.update();
        gmmp.computePhysics();


        // - - DRAWING - -
        window.clear();
        gmmp.render(window, camera);
        camera.drawCrosshairIfNeeded(window);
        window.display();
    }

    return 0;
}