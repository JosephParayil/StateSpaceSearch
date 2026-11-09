#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "../cpp-util/str.hpp"



struct GraphMM {
    struct Node {
        std::string title;
        std::string body;

        bool operator==(const Node& other) const {
            return lowercaseComparison(body, other.body);
        }

        virtual ~Node() = default;
    };
    std::unordered_map<std::string, std::unique_ptr<Node>, 
        CaseInsensitiveHash, CaseInsensitiveEqual> nodes;

    struct Connection {
        std::string nodeA;
        std::string nodeB;

        bool operator==(const Connection& other) const {
            return  (lowercaseComparison(nodeA, other.nodeA) && lowercaseComparison(nodeB, other.nodeB)) || (lowercaseComparison(nodeA, other.nodeB) && lowercaseComparison(nodeB, other.nodeA));
        }

        bool involves(const std::string& nodeTitle) const {
            return lowercaseComparison(nodeA, nodeTitle) || 
                lowercaseComparison(nodeB, nodeTitle);
        }

        virtual ~Connection() = default;
    };
    std::vector<std::unique_ptr<Connection>> connections;


     // = = = = ADDING & REMOVAL METHODS = = = = 
    //NOTE: capitalizaiton does not matter for titles

    bool nodeExists(std::string title) {
        return nodes.find(title) != nodes.end();
    }

    virtual bool addNode(std::string title) {
        if (nodeExists(title)) return false;
        
        nodes[title] = std::make_unique<Node>();
        nodes[title]->title = title;

        return true;
    }


    virtual bool removeNode(std::string title) {
        auto it = nodes.find(title);
        if (it == nodes.end()) {
            return false;
        }

        nodes.erase(it);
        connections.erase(
            std::remove_if(connections.begin(), connections.end(),
                [&](const std::unique_ptr<Connection>& conn) {
                    return conn->involves(title);
                }),
            connections.end()
        );

        return true;
    }

        virtual bool changeNodeTitle(std::string oldTitle, std::string newTitle, bool edit_connections = true) {
            //Conditions: oldTitle must exist, newTitle must not exist already
            //Exception: if we are simply changing the capitalization, things should be ok
            auto it0 = nodes.find(oldTitle);
            auto it1 = nodes.find(newTitle);
            if (!lowercaseComparison(oldTitle, newTitle) && (it0 == nodes.end() || it1 != nodes.end())) {
                return false;
            }
            
            

            auto nodeHandle = nodes.extract(it0);
            nodeHandle.key() = newTitle;
            nodeHandle.mapped()->title = newTitle;
            nodes.insert(std::move(nodeHandle));

            if (edit_connections) {
                for (auto& conPtr : connections) {
                    if (conPtr->nodeA == oldTitle) conPtr->nodeA = newTitle;
                    if (conPtr->nodeB == oldTitle) conPtr->nodeB = newTitle;
                }
            }

            return true;
        }

    virtual bool addConnection(std::string nodeA, std::string nodeB) {
        std::cout << "hello\n";
        // Conditions: Both nodes must exist; no self-connections, connection can't already exist
        
        if (!nodeExists(nodeA) || !nodeExists(nodeB) || lowercaseComparison(nodeA, nodeB)) {
            /*
            std::cout << "nodeA: " << nodeA << std::endl;
            std::cout << "nodeB: " << nodeB << std::endl;
            std::cout << "nodeExists(nodeA): " << nodeExists(nodeA) << std::endl;
            std::cout << "nodeExists(nodeB): " << nodeExists(nodeB) << std::endl;
            std::cout << "lowercaseComparison(nodeA, nodeB): " << lowercaseComparison(nodeA, nodeB) << std::endl;
            //*/
            return false;
        }
        
        // Checking if conncetion already exists
        Connection newEdge;
        newEdge.nodeA = nodeA;
        newEdge.nodeB = nodeB;

        for (const auto& conn : connections) {
            if (*conn == newEdge) { 
                /*
                std::cout << "nodeA: " << nodeA << std::endl;
                std::cout << "nodeB: " << nodeB << std::endl;
                //*/
                return false;
            }
        }

        connections.push_back(std::make_unique<Connection>(newEdge));
        return true;
    }
    

    virtual void printNodes() {
        std::cout << "= NODES =: \n";
        for (auto& [title, nodePtr] : nodes) {
            std::cout << title << ": " << nodePtr->body << std::endl;
            //std::cout << std::endl;
        }
    }

    virtual void printConnections() const {
        std::cout << "= CONNECTIONS =: \n";
        size_t i = 0;
        for (auto& conn : connections) {
            std::cout << i << ": (" << conn->nodeA << ", " << conn->nodeB << ")" << std::endl;
            //std::cout << std::endl;
            i++;
        }
    }

    void print() {
        printNodes();
        //std::cout << std::endl;
        printConnections();
    }
};