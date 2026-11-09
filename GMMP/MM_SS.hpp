#include <functional>
#include <unordered_set>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "../sfml-util/sfml_util.hpp"

#include "MM_3D.hpp"



struct MM_SS : MM_3D { 
    /* Body inherited from GraphMM will be the state
    struct NodeSS : Node3D {};
    */


    /* Constructor:
        - Takes in template GUI window
        - Takes in function to manipulate given GUI window based on string serialized state
    */ 

    tgui::Gui& gui;
    tgui::ChildWindow& templateStateWindow;
    std::function<void(tgui::ChildWindow&, std::string)> configureStateWindow; 

    Node3D* selectedNode = nullptr;
    sf::Vector2f mouse_pos_cache{-1,-1};

    bool drawLabels = false;

    std::unordered_set<std::string> special_nodes;

    MM_SS(std::vector<std::string> states, std::vector<std::pair<size_t, size_t>> actions, tgui::Gui& gui,
        tgui::ChildWindow& templateStateWindow, std::function<void(tgui::ChildWindow&, std::string)> configureStateWindow, 
        std::vector<int> special_states = {}, bool drawLabels = false)     
    : gui(gui), templateStateWindow(templateStateWindow), configureStateWindow(configureStateWindow), drawLabels(drawLabels){
        
        for (std::string state : states) {
            assert(addNode(state));
            nodes[state]->body = state;
        }

        for (auto& action : actions) {
            assert(addConnection(states[action.first], states[action.second]));
        }

        templateStateWindow.setVisible(false);
        templateStateWindow.setCloseBehavior(tgui::ChildWindow::CloseBehavior::Hide);
        templateStateWindow.onClose([&]() {
            selectedNode = nullptr;
        });

        
        for (int special_state : special_states) {
            special_nodes.insert(states[special_state]);
        }

        update3DObjects();
    }

    

    std::string handleEvent(sf::RenderWindow& window, const std::optional<sf::Event>& event, Camera& camera) {
        MM_3D::handleEvent(window, event);

        bool tgui_consumed = gui.handleEvent(*event);

        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (!tgui_consumed && mouseButtonPressed->button == sf::Mouse::Button::Left) { 
                Node3D* newSelectedNode = nullptr;
                for (auto& [title, nodePtr] : nodes) {
                    Node3D* node3D = toNode3D(nodePtr);
                    if (node3D->selected) {
                        newSelectedNode = node3D;
                        break;
                    }
                }

                if (newSelectedNode) {
                    selectedNode = newSelectedNode;
                    openGuiForSelectedNode(window);
                } else {
                    mouse_pos_cache = sf::Vector2f(sf::Mouse::getPosition(window));
                }
                camera.mouseLocked = false; //in both cases (inspecting or scanning, we want to do this)
            }
        } else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouse_pos_cache.x != -1) {
                sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
                updateSelectionOnRect(window, camera, getSelectionRect(mousePos, mouse_pos_cache));
                camera.mouseLocked = false;

                mouse_pos_cache.x = mouse_pos_cache.y = -1;
            }
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scan::Escape) {
                templateStateWindow.close();
            }
        }

        
        if (selectedNode) return "IN_GUI";
        else return "";
    } 

    void render(sf::RenderWindow& window, Camera& camera) {
            //Partial depth sorting
            bool draw_selected = false;
            for (int sweep =0; sweep<2; sweep++) {
                for (auto& [title, nodePtr] : nodes) {
                    Node3D* node3D = toNode3D(nodePtr);
                    if (draw_selected == node3D->selected) {
                        sf::Color color = special_nodes.contains(title) ? sf::Color::Red : sf::Color::White;
                        node3D->sphere.draw(window, camera, (draw_selected ? sf::Color::Blue : color));
                        if(drawLabels) {
                            node3D->label.draw(window, camera, (draw_selected ? sf::Color::Blue : color));
                            //Needed in Windows to prevent font drawing from corrupting everything else
                            //More efficient way to do this if you draw all the fonts, run this once, and draw everything else
                            window.resetGLStates();
                        }
                        
                    }

                }

                for (auto& connectionPtr : connections) {
                    Connection3D* con3D = toConnection3D(connectionPtr);
                    if (draw_selected == con3D->selected) {
                        con3D->line.draw(window, camera, (draw_selected ? sf::Color::Blue : sf::Color::White));
                    }
                }

                draw_selected = !draw_selected;
            }
        
        if (selectedNode) {
            tgui::Vector2f ui_pos = templateStateWindow.getPosition();
            draw3DLineTo2DPoint(window, selectedNode->position, sf::Vector2f(ui_pos.x, ui_pos.y), camera, LINE_LABEL_THICKNESS, LINE_LABEL_COLOR);
        }

        if (mouse_pos_cache.x != -1) {
            sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
            sf::FloatRect rect =  getSelectionRect(mousePos, mouse_pos_cache);
            sf::RectangleShape outline(rect.size);
            outline.setPosition(rect.position);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor(LINE_LABEL_COLOR); // or whatever color
            outline.setOutlineThickness(LINE_LABEL_THICKNESS);

            window.draw(outline);

        }
        gui.draw();
        
    }

    void openGuiForSelectedNode(sf::RenderWindow& window) {
        configureStateWindow(templateStateWindow, selectedNode->body);
        sf::Vector2f ui_pos = sf::Vector2f(window.getSize().x * 0.1, window.getSize().y * 0.1);//rand_2d_pos(window, 0.2);
        templateStateWindow.setPosition(ui_pos.x, ui_pos.y);
        templateStateWindow.setVisible(true);
    }

    void updateSelectionOnPoint(sf::RenderWindow& window, Camera& camera, sf::Vector2f pos) {
        MM_3D::updateSelectionOnPoint(window, camera, pos, drawLabels, false);
    }

    void updateSelectionOnRect(sf::RenderWindow& window, Camera& camera, sf::FloatRect rect) {
        //1. We do not select connections 2. Select only one node, that being the closest
        MM_3D::updateSelectionOnRect(window, camera, rect, false);

        Node3D* closestSelectedNode = nullptr;
        for (auto& [title, nodePtr] : nodes) {
            Node3D* node3D = toNode3D(nodePtr);
            if (node3D->selected) {
                if (!closestSelectedNode || node3D->sphere.getDistance(camera) < closestSelectedNode->sphere.getDistance(camera)) {
                    closestSelectedNode = node3D;
                }
            }
        } 

        if (closestSelectedNode) {
            selectedNode = closestSelectedNode;
            openGuiForSelectedNode(window);
        }

        //Setting everything else unselected;
        for (auto& [title, nodePtr] : nodes) {
            Node3D* node3D = toNode3D(nodePtr);
            node3D->selected = node3D == closestSelectedNode;
        }
    }


};