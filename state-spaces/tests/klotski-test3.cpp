#include <iostream>
#include "../klotski.hpp"


int main() {
    KlotskiBoard klotski_board(6,6);


    klotski_board.addRect(KlotskiRect(2,0,1,3, KlotskiRect::MovementMode::Vertical));
    klotski_board.addRect(KlotskiRect(5,1,1,2, KlotskiRect::MovementMode::Vertical));
    klotski_board.addRect(KlotskiRect(5,3,1,3, KlotskiRect::MovementMode::Vertical));

    klotski_board.addRect(KlotskiRect(3,0,3,1, KlotskiRect::MovementMode::Horizontal));
    klotski_board.addRect(KlotskiRect(0,2,2,1, KlotskiRect::MovementMode::Horizontal));
    klotski_board.addRect(KlotskiRect(1,3,2,1, KlotskiRect::MovementMode::Horizontal));
    klotski_board.addRect(KlotskiRect(2,5,3,1, KlotskiRect::MovementMode::Horizontal));



    klotski_board.print();

    std::cout << "Listen: ";
    std::cout << klotski_board.probeAction(1, 0) << std::endl;
    std::cout << klotski_board.probeAction(1, 1) << std::endl;
    std::cout << klotski_board.probeAction(1, 2) << std::endl;
    std::cout << klotski_board.probeAction(1, 3) << std::endl;
    std::cout << std::endl;
    std::cout << klotski_board.blocks[1].pos.x << ' ' << klotski_board.blocks[1].pos.y;
    std::cout << std::endl;

    return 0;
}