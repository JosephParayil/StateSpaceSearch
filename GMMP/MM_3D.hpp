#pragma once

#include <fstream>

#include <SFML/Graphics.hpp>

#include "../sfml-3d/3d_camera.hpp"
#include "../sfml-3d/3d_engine.hpp"
#include "../sfml-3d/math4.hpp"

#include "GraphMM.hpp"


const sf::Font LABEL_FONT("GMMP/assets/JetBrainsMonoNerdFont-Medium.ttf");
const vec4 LABEL_OFFSET(0, 3, 0);

//Not used here but in subclasses
const sf::Color LINE_LABEL_COLOR = sf::Color(128, 128, 128);
const float LINE_LABEL_THICKNESS = 3.0f;


struct MM_3D : GraphMM {
    bool physics_paused = false;
    

    struct Node3D : Node {
        vec4 position;
        vec4 velocity;
        Sphere3D sphere;
        Label3D label;

        bool selected = false;
    };

    struct Connection3D : Connection {
        Line3D line;

        bool selected = false;
    };


    static vec4 rand_position() {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        static std::uniform_real_distribution<float> dist(-100.0f, 100.0f);

        return vec4(dist(gen), dist(gen), dist(gen));
    }
    
    Node3D* toNode3D(std::unique_ptr<Node>& nodePtr) {
        Node3D* node3D = static_cast<Node3D*>(nodePtr.get());
        assert(node3D);
        return node3D;
    }

    Connection3D* toConnection3D(std::unique_ptr<Connection>& conPtr) {
        Connection3D* con3D = static_cast<Connection3D*>(conPtr.get());
        assert(con3D);
        return con3D;
    }


    // = = = = UPDATE & RENDERING METHODS = = = =

    void update3DObjects() {
        
        for (auto& [title, nodePtr] : nodes) {
            Node3D* node3D = toNode3D(nodePtr);
            node3D->sphere.position = node3D->position;
            node3D->label.position = node3D->position + LABEL_OFFSET;
        }

        for (auto& connectionPtr : connections) {
            Connection3D* con3D = toConnection3D(connectionPtr);
            con3D->line.a = toNode3D(nodes[con3D->nodeA])->position;
            con3D->line.b = toNode3D(nodes[con3D->nodeB])->position;
        }
    }


    void render(sf::RenderWindow& window, Camera& camera, bool drawLabels = true) {
        //Partial depth sorting

        bool draw_selected = false;
        for (int sweep =0; sweep<2; sweep++) {
            for (auto& [title, nodePtr] : nodes) {
                Node3D* node3D = toNode3D(nodePtr);
                if (draw_selected == node3D->selected) {
                    node3D->sphere.draw(window, camera, (draw_selected ? sf::Color::Blue : sf::Color::White));
                    if(drawLabels) {
                        node3D->label.draw(window, camera, (draw_selected ? sf::Color::Blue : sf::Color::White));
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
    }

    void computePhysics() {
        if (physics_paused) {
            update3DObjects();
            return;
        }
        /*
        Forces:
        - Attractive connection force (hooke's law)
        - Repelling node force (columb's law)
        - Dampening friction force
        - Bounding attractive force
        */
        const float everything_constant = 10;

        const float hooke_K = 0.01 * everything_constant; // 0.001
        const float columb_K = 50 * everything_constant;
        const float dampening_constant = std::pow(0.9, everything_constant);
        const float bounding_constant = 0 * everything_constant; //0.00001

        //Update velocities
        for (auto& connectionPtr : connections) {
            Connection3D* con3D = toConnection3D(connectionPtr);
            Node3D* nodeA = toNode3D(nodes[con3D->nodeA]);
            Node3D* nodeB = toNode3D(nodes[con3D->nodeB]);

            vec4& pos1 = nodeA->position;
            vec4& pos2 = nodeB->position;
            nodeA->velocity += (pos2 - pos1)*hooke_K;
            nodeB->velocity += (pos1 - pos2)*hooke_K;
        }

        for (auto& [title, nodePtrI]: nodes) {
            Node3D* nodeI = toNode3D(nodePtrI);
            //Columb's force + bounding attractive force
            for (auto& [title, nodePtrJ]: nodes) {
                if (nodePtrI==nodePtrJ) continue;
                Node3D* nodeJ = toNode3D(nodePtrJ);


                float x_dist = nodeI->position.x-nodeJ->position.x;
                float y_dist = nodeI->position.y-nodeJ->position.y;
                float z_dist = nodeI->position.z-nodeJ->position.z;

                float r_cubed = pow(pow(x_dist, 2) + pow(y_dist, 2) + pow(z_dist, 2), 1.5);
                if (r_cubed <= 0.1)  {
                    r_cubed = 1;
                    nodeI->position += rand_vec4(-1,1).normalize();
                }
                vec4 columb_factor(columb_K*x_dist/r_cubed, columb_K*y_dist/r_cubed, columb_K*z_dist/r_cubed);
                vec4 bounding_factor(-bounding_constant * x_dist, -bounding_constant * y_dist, -bounding_constant * z_dist);
                nodeI->velocity += columb_factor + bounding_factor;
            }
            
            //Dampening force
            nodeI->velocity *= dampening_constant;
           
        }

        //Update positions
        for (auto& [title, nodePtr]: nodes) {
            Node3D* node3D = toNode3D(nodePtr);
            node3D->position += node3D->velocity;
        }

        //Update objects
        update3DObjects();
    }


    void updateSelectionOnPoint(sf::RenderWindow& window, Camera& camera, sf::Vector2f pos, bool selectLabels = true, bool selectConnections = true) {
        if (selectConnections) for (auto& connectionPtr : connections) {
            Connection3D* con3D = toConnection3D(connectionPtr);
            con3D->selected = false;
        }

        Node3D* closest_node = nullptr;
        for (auto& [title, nodePtr] : nodes) {
            Node3D* node3D = toNode3D(nodePtr);
            node3D->selected = false;

            auto shape = node3D->sphere.computeShape(window, camera);
            std::unique_ptr<Shape2D> label2D = nullptr;
            if(selectLabels) label2D = node3D->label.computeShape(window, camera);

            if (!shape || (selectLabels && !label2D)) continue;

            if (shape->computeCollisionWithPoint(pos) || (selectLabels && label2D->computeCollisionWithPoint(pos))) {
                if (!closest_node || node3D->sphere.getDistance(camera) < closest_node->sphere.getDistance(camera)) {
                    closest_node = node3D;
                }
            }            
        }
        if (closest_node) {
            closest_node->selected = true;
            return;
        }

        if (!selectConnections) return;
        Connection3D* closest_connection = nullptr;
        for (auto& connectionPtr : connections) {
            Connection3D* con3D = toConnection3D(connectionPtr);

            auto shape = con3D->line.computeShape(window, camera);
            if (!shape) continue;

            if (shape->computeCollisionWithPoint(pos)) {
                if (!closest_connection || con3D->line.getDistance(camera) < closest_connection->line.getDistance(camera)) {
                    closest_connection = con3D;
                    con3D->selected = true;
                }
            }    
        }
    }

    void updateSelectionOnRect(sf::RenderWindow& window, Camera& camera, sf::FloatRect rect, bool selectConnections = true) {
        for (auto& [title, nodePtr] : nodes) {
            Node3D* node3D = toNode3D(nodePtr);
            auto shape = node3D->sphere.computeShape(window, camera);
            if (!shape) continue;

            Circle2D* circle2D = static_cast<Circle2D*>(shape.get());
            node3D->selected = rect.contains(circle2D->center);
            
        }

        
        if (selectConnections) for (auto& connectionPtr : connections) {
            Connection3D* con3D = toConnection3D(connectionPtr);
            auto shape = con3D->line.computeShape(window, camera);
            if (!shape) continue;

            Line2D* line2D = static_cast<Line2D*>(shape.get());
            con3D->selected = lineRectIntersect(line2D->a, line2D->b, rect);

        }
    }
    
    
    std::string handleEvent(sf::RenderWindow& window, const std::optional<sf::Event>& event) {
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scan::Space) {
                physics_paused = !physics_paused;
            }
        }

        return "";
    }


    // = = = = ADDING & REMOVAL METHODS = = = = 

    void build3DNode(Node3D* newNode, const std::string& title) {
        newNode->position = rand_position();
        newNode->sphere.position = newNode->position;
        newNode->sphere.radius = 1;
        newNode->label = Label3D(
            newNode->position + LABEL_OFFSET,
            title,
            LABEL_FONT
        );
        newNode->title = title;
    }

    virtual bool addNode(std::string title) override {
        if (nodeExists(title)) return false;

        auto newNode = std::make_unique<Node3D>();
        build3DNode(newNode.get(), title);
        nodes[title] = std::move(newNode);

        return true;
    }

    bool addNodeAt(std::string title, vec4 pos) {
        if (!addNode(title)) return false;
        toNode3D(nodes[title])->position = pos;
        return true;
    }

    virtual bool changeNodeTitle(std::string oldTitle, std::string newTitle, bool edit_connections = true) override {
        if (!GraphMM::changeNodeTitle(oldTitle, newTitle, edit_connections)) return false;

        Node3D* node3D = toNode3D(nodes[newTitle]);
        node3D->label.text = newTitle;

        return true;
    }

    virtual bool addConnection(std::string nodeA, std::string nodeB) override {
        // Conditions: Both nodes must exist; no self-connections, connection can't already exist
        if (!nodeExists(nodeA) || !nodeExists(nodeB) || lowercaseComparison(nodeA, nodeB)) {
            //*
            std::cout << "nodeA: " << nodeA << std::endl;
            std::cout << "nodeB: " << nodeB << std::endl;
            std::cout << "nodeExists(nodeA): " << nodeExists(nodeA) << std::endl;
            std::cout << "nodeExists(nodeB): " << nodeExists(nodeB) << std::endl;
            std::cout << "lowercaseComparison(nodeA, nodeB): " << lowercaseComparison(nodeA, nodeB) << std::endl;
            //*/
            return false;
        }
        
        auto newConn = std::make_unique<Connection3D>();
        newConn->nodeA = nodeA;
        newConn->nodeB = nodeB;
        newConn->line.thickness = 1.f;
        
        for (const auto& conn : connections) {
            if (*conn == *newConn) {
                //*
                std::cout << "nodeA: " << nodeA << std::endl;
                std::cout << "nodeB: " << nodeB << std::endl;
                //*/ 
                return false;
            }
        }

        connections.push_back(std::move(newConn));
        return true;
    }


    // = = = = FILE IO = = = = 
    void savePositions(const std::string& filename) {
        std::ofstream ofs(filename, std::ios::binary);
        if (!ofs) return;

        // 1. Write the total number of nodes so we know when to stop reading
        size_t nodeCount = nodes.size();
        ofs.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));

        for (const auto& [title, nodePtr] : nodes) {
            // Cast to our derived type to get the position
            Node3D* n3d = static_cast<Node3D*>(nodePtr.get());

            // 2. Write string length then the string characters
            size_t nameLen = title.size();
            ofs.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
            ofs.write(title.data(), nameLen);

            // 3. Write the vec4 position data
            ofs.write(reinterpret_cast<const char*>(&n3d->position), sizeof(vec4));
        }
    }

    void loadPositions(const std::string& filename) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs) return;

        size_t nodeCount = 0;
        ifs.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

        for (size_t i = 0; i < nodeCount; ++i) {
            // 1. Read the name length and the name string
            size_t nameLen = 0;
            ifs.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            
            std::string title(nameLen, '\0');
            ifs.read(&title[0], nameLen);

            // 2. Read the vec4 position
            vec4 pos;
            ifs.read(reinterpret_cast<char*>(&pos), sizeof(vec4));

            // 3. Map it back to the node
            Node3D* n3d = toNode3D(nodes[title]);
            if (n3d) {
                n3d->position = pos;
            } else {
                // Optional: If the node doesn't exist in the current graph, 
                // you could call addNode(title) here if you want to reconstruct structure.
            }
        }
    }


    void printPositions() {
        for (auto& [title, nodePtr] : nodes) {
            Node3D* n3d = toNode3D(nodes[title]);
            std::cout << title << ": " << n3d->position.string() << std::endl;
            //std::cout << std::endl;
        }
    }

};