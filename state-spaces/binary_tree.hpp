#pragma once

#include "ss.hpp"
#include <cmath>

StateSpace binary_tree_ss(size_t depth) {
    StateSpace ss;
    ss.NUM_NODES = std::pow(2, depth) -1;


    size_t i=1;
    for (size_t x = 1; x < depth; x++) { //skip depth==0 because it accepts no edges
        size_t next_level_first_index = std::pow(2, x+1)-1;
        while (i < next_level_first_index) {            
            //Detect 'parent' index
            size_t parent_i = (i-1)/2;

            ss.edges.emplace_back(parent_i, i);
            i++;
        }
    }

    

    return ss;
}





