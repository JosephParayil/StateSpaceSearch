#include <iostream>
#include "../klotski.hpp"




int main() {
    KlotskiBoard board(5, 5);

    KlotskiRect rect(0, 0, 2, 1, KlotskiRect::MovementMode::Both);
    KlotskiRect rect2(1, 1, 3, 4, KlotskiRect::MovementMode::Both);

    board.addRect(rect);
    board.addRect(rect2);


    if (board==board.calculateNewStates()[2].calculateNewStates()[3]) {
        std::cout << "Both equal\n";
    }

    /*
    std::cout << "= = = = = = = = ORIGINAL = = = = = = = =\n";
    board.print();

    auto new_states = board.calculateNewStates();

    for (size_t i=0;i<new_states.size();i++) {
        std::cout << i << " = = = = = = = =\n";
        new_states[i].print();
        std::cout << std::endl; 
    }


    //std::cout << board.blocks.size() << std::endl;

    */
    return 0;
}