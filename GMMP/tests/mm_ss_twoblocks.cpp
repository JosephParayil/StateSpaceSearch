
#include <iostream>
#include <cassert>
#include <random>

#include <SFML/Graphics.hpp>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <sfml-3d/3d_camera.hpp>
#include <sfml-3d/3d_engine.hpp>
#include <sfml-3d/math4.hpp>
#include <state-spaces/ss.hpp>
#include <state-spaces/two_blocks.hpp>


#include "GraphMM.hpp"
#include "MM_3D.hpp"
#include "MM_SS.hpp"



//State format: [INT]-[INT]
void configureStateWindow_twoblocks(tgui::ChildWindow& window, std::string state) {
    auto widgetA = window.get<tgui::Panel>("A");
    auto widgetB = window.get<tgui::Panel>("B");

    int first = 0;
    int second = 0;

    std::size_t dashPos = state.find('-');
    if (dashPos != std::string::npos) {
        first = std::stoi(state.substr(0, dashPos));
        second = std::stoi(state.substr(dashPos + 1));
    }

    widgetA->setPosition(widgetA->getPosition().x, 50*first);
    widgetB->setPosition(widgetB->getPosition().x, 50*second);
}




int main() {
    const int HEIGHT = 1400;
    const int WIDTH = 2100;
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Mental Modeller");
    Camera camera(window, 60.0f, 0.001f, 2.f, 10.f, 0.5f); 
    

    const std::string STATE_GUI_PATH = "assets/twoblocks_state_gui.txt";
    tgui::Gui gui(window);
    gui.loadWidgetsFromFile(STATE_GUI_PATH);
    auto templateWindow = gui.get<tgui::ChildWindow>("twoblocks");


    StateSpace my_two_blocks_ss = two_blocks_ss();
    std::vector<std::string> states;
    for (int state = 0; state< my_two_blocks_ss.NUM_NODES; state++) {
        auto [a, b] = unserialize_two_blocks_state(state);


        states.push_back(std::to_string(a) + "-" + std::to_string(b));
    }



    ///*
    MM_SS my3DMM(states, my_two_blocks_ss.edges, gui, *templateWindow, configureStateWindow_twoblocks);
    
    
    std::cout << "mm created" << std::endl;



    bool IN_GUI = false;
    bool WAS_IN_GUI = false;
    while (window.isOpen()) {
        my3DMM.updateSelectionOnPoint(window, camera, sf::Vector2f(sf::Mouse::getPosition(window)));
        while (const auto& event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else {
                std::string handler_code = my3DMM.handleEvent(window, event, camera);
                IN_GUI = handler_code == "IN_GUI";
                //if (!WAS_IN_GUI) 
                camera.handleEvent(event);
                
                   
            }   
        } 
        // - - UPDATES - - 
        if (true) {//(!WAS_IN_GUI) {
            camera.update();
        }
        WAS_IN_GUI = IN_GUI;
        
        my3DMM.computePhysics();


        // - - DRAWING - -
        window.clear();
        my3DMM.render(window, camera);
        camera.drawCrosshairIfNeeded(window);
        window.display();
    }
    //*/

    return 0;
}