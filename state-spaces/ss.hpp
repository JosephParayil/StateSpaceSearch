#pragma once


#include <iostream>
#include <vector>
#include <utility>


bool unordered_pair_exists(std::vector<std::pair<size_t, size_t>>& edges, size_t a, size_t b) {
    for (auto pair : edges) {
        if ((pair.first == a && pair.second == b) || (pair.second == a && pair.first == b)) {
            return true;
        }
    }

    return false;
}


struct StateSpace {
    size_t NUM_NODES;
    std::vector<std::pair<size_t, size_t>> edges;

    void print() const {
        std::cout << "Node count: " << NUM_NODES << std::endl;
        std::cout << "Connections:\n";
        for (size_t i=0;i <edges.size(); i++) {
            std::cout << "\t{" << edges[i].first << ", " << edges[i].second << "}\n"; 
        }
    }

    bool connectionExists(size_t a, size_t b) {
        return unordered_pair_exists(edges, a, b);
    }
};


