#include <iostream>
#include "../klotski.hpp"




int main() {
    KlotskiBoard board(3,3);

    KlotskiRect rect0(0,0,2,2, KlotskiRect::MovementMode::Both);
    board.addRect(rect0);
    KlotskiRect rect1(2,2,1,1, KlotskiRect::MovementMode::Both);
    board.addRect(rect1);
    
    

    std::vector<KlotskiBoard> board_space;
    board_space.push_back(board);
    
    searchKlotskiStateSpace(board_space, {});


    for (int i =0; i<board_space.size(); i++) {
        std::cout << "BoardID: " << i << "= = = = = = = = = =\n";
        board_space[i].print();
        std::cout << std::endl;
    }

    return 0;

}

