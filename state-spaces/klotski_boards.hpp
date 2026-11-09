#include "klotski.hpp"

#include <vector>
#include <utility>
#include <string>


std::pair<std::vector<KlotskiBoard>, std::vector<std::string>> create_board_list() {
    std::vector<KlotskiBoard> boards;
    std::vector<std::string> board_representations;
    
    {
    KlotskiBoard klotski_board(3,3);
    KlotskiRect rect0(0,0,2,2, KlotskiRect::MovementMode::Both);
    klotski_board.addRect(rect0);
    KlotskiRect rect1(2,2,1,1, KlotskiRect::MovementMode::Both);
    klotski_board.addRect(rect1);

    boards.push_back(klotski_board);
    board_representations.push_back(
R"(
0	0	_
0	0	_
_	_	1
)");
    }
    {
    KlotskiBoard klotski_board(7,7);

    KlotskiRect rect0(3,0,1,2, KlotskiRect::MovementMode::Vertical);
    klotski_board.addRect(rect0);
    KlotskiRect rect1(0,3,2,1, KlotskiRect::MovementMode::Horizontal);
    klotski_board.addRect(rect1);

    boards.push_back(klotski_board);
    board_representations.push_back(
R"(
_	_	_	0	_	_	_
_	_	_	0	_	_	_
_	_	_	_	_	_	_
1	1	_	_	_	_	_
_	_	_	_	_	_	_
_	_	_	_	_	_	_
_	_	_	_	_	_	_
)");
    }
    {
        KlotskiBoard klotski_board(7,7);

        KlotskiRect rect0(3,0,1,2, KlotskiRect::MovementMode::Vertical);
        klotski_board.addRect(rect0);
        KlotskiRect rect1(0,3,2,1, KlotskiRect::MovementMode::Horizontal);
        klotski_board.addRect(rect1);
        KlotskiRect rect2(5,4,2,1, KlotskiRect::MovementMode::Horizontal);
        klotski_board.addRect(rect2);

        boards.push_back(klotski_board);
        board_representations.push_back(
R"(
_	_	_	0	_	_	_
_	_	_	0	_	_	_
_	_	_	_	_	_	_
1	1	_	_	_	_	_
_	_	_	_	_	2	2
_	_	_	_	_	_	_
_	_	_	_	_	_	_
)");
    }
    {
        KlotskiBoard klotski_board(6,6);

        klotski_board.addRect(KlotskiRect(1,0,1,2, KlotskiRect::MovementMode::Vertical));	//0
        klotski_board.addRect(KlotskiRect(2,0,3,1, KlotskiRect::MovementMode::Horizontal));	//1
        klotski_board.addRect(KlotskiRect(5,0,1,2, KlotskiRect::MovementMode::Vertical));	//2
        klotski_board.addRect(KlotskiRect(5,2,1,2, KlotskiRect::MovementMode::Vertical));	//3
        klotski_board.addRect(KlotskiRect(4,2,1,2, KlotskiRect::MovementMode::Vertical));	//4
        klotski_board.addRect(KlotskiRect(4,4,2,1, KlotskiRect::MovementMode::Horizontal));	//5
        klotski_board.addRect(KlotskiRect(3,5,3,1, KlotskiRect::MovementMode::Horizontal));	//6
        klotski_board.addRect(KlotskiRect(2,4,1,2, KlotskiRect::MovementMode::Vertical));	//7
        klotski_board.addRect(KlotskiRect(0,4,1,2, KlotskiRect::MovementMode::Vertical));	//8
        klotski_board.addRect(KlotskiRect(0,3,2,1, KlotskiRect::MovementMode::Horizontal));	//9
        klotski_board.addRect(KlotskiRect(0,2,2,1, KlotskiRect::MovementMode::Horizontal));	//10
        klotski_board.addRect(KlotskiRect(2,1,1,2, KlotskiRect::MovementMode::Vertical));	//11
        klotski_board.addRect(KlotskiRect(2,3,2,1, KlotskiRect::MovementMode::Horizontal));	//12
        
        boards.push_back(klotski_board);
        board_representations.push_back(
R"(
_	0	1	1	1	2
_	0	11	_	_	2
10	10	11	_	4	3
9	9	12	12	4	3
8	_	7	_	5	5
8	_	7	6	6	6

)");
    }
    {
        KlotskiBoard klotski_board(6,6);

        klotski_board.addRect(KlotskiRect(2,0,1,3, KlotskiRect::MovementMode::Vertical));
        klotski_board.addRect(KlotskiRect(5,1,1,2, KlotskiRect::MovementMode::Vertical));
        klotski_board.addRect(KlotskiRect(5,3,1,3, KlotskiRect::MovementMode::Vertical));

        klotski_board.addRect(KlotskiRect(3,0,3,1, KlotskiRect::MovementMode::Horizontal));
        klotski_board.addRect(KlotskiRect(0,2,2,1, KlotskiRect::MovementMode::Horizontal));
        klotski_board.addRect(KlotskiRect(1,3,2,1, KlotskiRect::MovementMode::Horizontal));
        klotski_board.addRect(KlotskiRect(2,5,3,1, KlotskiRect::MovementMode::Horizontal));

        boards.push_back(klotski_board);
        board_representations.push_back(
R"(
_	_	0	3	3	3
_	_	0	_	_	1
4	4	0	_	_	1
_	5	5	_	_	2
_	_	_	_	_	2
_	_	6	6	6	2
)");
    }


    return {boards, board_representations};
}






