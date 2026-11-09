#pragma once

#include <iostream>
#include <vector>



struct Graph { //Undirected
    std::vector<std::vector<std::pair<int, int>>> adjacent_nodes;

    void add_node() {
        adjacent_nodes.push_back({});
    }

    bool is_connected(int i, int j) const {
        for (auto pair : adjacent_nodes[i]) {
            if (pair.first == j) return true;
        }
        return false;
    }

    void add_connection(int i, int j, int weight=1) {
        if (is_connected(i, j)) return;
        adjacent_nodes[i].push_back({j, weight});
        adjacent_nodes[j].push_back({i, weight});
    }


    void print() const {
        for (int i = 0; i < adjacent_nodes.size(); i++) {
            std::cout << i << ": " << std::endl;
            for (auto pair : adjacent_nodes[i]) {
                std::cout << " <-> " << pair.first << " (Weight = " << pair.second << ")\n";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    int size() const {
        return adjacent_nodes.size();
    }
};