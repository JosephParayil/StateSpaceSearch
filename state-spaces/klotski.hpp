#pragma once

#include "ss.hpp"
#include <cassert>
#include <string>

/*
Klotski:

Board consists of rectangles:
    Rectangles
        - x and y width
        - x y coords
        - movement mode (Horizontal, vertical, both)


    Coordinate 0,0 is at the top-left

    Action space for individual rect:
        0: UP (y-1)
        1: LEFT (x-1)
        2: DOWN (y+1)
        3: RIGHT (x+1)
    
        If movement mode is Horizontal
            0->1, 2->3
        If movement mode is vertical
            1->0, 3->2
    
*/



struct KlotskiRect {
    struct Coord {
        int x; int y;
        Coord(int x, int y) : x(x), y(y) {}

        Coord& operator+=(Coord& other) {
            x += other.x; y += other.y;
            return *this;
        }

        Coord operator+(Coord& other) {
            return Coord(x+other.x, y+other.y);
        }

        Coord& operator*=(int s) {
            x *= s;
            y *= s;
            return *this;
        }

        bool operator==(const Coord& other) const {
            return x==other.x && y==other.y;
        }
    };

    Coord size;
    Coord pos;

    enum MovementMode {
        Both,
        Horizontal,
        Vertical
    };
    MovementMode movementMode;


    KlotskiRect(int x_pos, int y_pos, int width, int height, MovementMode movementMode) : 
        pos(x_pos, y_pos), size(width, height), movementMode(movementMode) {
            assert(width >0 && height > 0);
    }


    bool containsPoint(int x, int y) const {
        return (pos.x <= x  && x < pos.x+size.x) && (pos.y <= y  && y < pos.y+size.y);
    }

    bool isColliding(const KlotskiRect& other) const {
        /*
        x lower bound:
            pos.x
        x upper bound:
            pos.x+size.x

        y lower bound:
            pos.y
        y upper bound:
            pos.y+size.y
        */
                            //Is my lower bound in the other rect                          or          is the other's lower bound in my rect
        bool x_intersect = (other.pos.x <= pos.x && pos.x < other.pos.x + other.size.x) || (pos.x <= other.pos.x && other.pos.x < pos.x + size.x);
        bool y_intersect = (other.pos.y <= pos.y && pos.y < other.pos.y + other.size.y) || (pos.y <= other.pos.y && other.pos.y < pos.y + size.y);
        
        return x_intersect && y_intersect;
    }

    bool isOutOfBounds(int board_width, int board_height) {
        return pos.x < 0 || pos.x + size.x > board_width 
            || pos.y < 0 || pos.y + size.y > board_height;
    }

    
    KlotskiRect operator+(Coord translation) const {
        KlotskiRect result = *this;
        result.pos += translation;
        return result;
    }

    KlotskiRect& operator+=(Coord translation) {
        pos += translation;
        return *this;
    }

    bool operator==(const KlotskiRect& other) const {
        return pos == other.pos && size == other.size && movementMode == other.movementMode;
    }

    bool checkCollisions(int width, int height, std::vector<KlotskiRect>& other_rects) {
        if (isOutOfBounds(width, height)) return true;

        for (KlotskiRect& other_rect : other_rects) {
            if (&other_rect != this && isColliding(other_rect)) {
                return true;
            }
        }

        return false;
    }

    int normalize_action(int action) {
        assert(action >= 0 && action < 4);
        if (movementMode == MovementMode::Horizontal) {
            if (action == 0 || action == 2) action++;
        } else if (movementMode == MovementMode::Vertical) {
            if (action == 1 || action == 3) action--;
        }
        return action;
    }

    //Same thing as executeAction, except no action is taken.
    bool probeAction(int action, int width, int height, std::vector<KlotskiRect>& other_rects) {
        action = normalize_action(action);

        switch (action) {
            case 0: {
                pos.y -=1;
                bool result = !checkCollisions(width, height, other_rects);  
                pos.y += 1;
                return result;
                break;
            } case 1: {
                pos.x -=1;
                bool result = !checkCollisions(width, height, other_rects);  
                pos.x += 1;
                return result;
                break;
            } case 2: {
                pos.y +=1;
                bool result = !checkCollisions(width, height, other_rects);  
                pos.y -= 1;
                return result;
                break;
            } case 3: {
                pos.x +=1;
                bool result = !checkCollisions(width, height, other_rects);  
                pos.x -= 1;
                return result;
            }
        }

        return false;
    }

    //returns false if movement failed due to collision
    bool executeAction(int action, int width, int height, std::vector<KlotskiRect>& other_rects) {
        action = normalize_action(action);

        switch (action) {
            case 0: {
                pos.y -=1;
                if (checkCollisions(width, height, other_rects)) {
                    pos.y += 1;
                    return false;
                }
                else return true;
            } case 1: {
                pos.x -=1;
                if (checkCollisions(width, height, other_rects)) {
                    pos.x += 1;
                    return false;
                }
                else return true;
            } case 2: {
                pos.y +=1;
                if (checkCollisions(width, height, other_rects)) {
                    pos.y -= 1;
                    return false;
                }
                else return true;
            } case 3: {
                pos.x +=1;
                if (checkCollisions(width, height, other_rects)) {
                    pos.x -= 1;
                    return false;
                }
                else return true;
            }
        }

        return false;
    }


    std::vector<KlotskiRect> calculateNewStates(int width, int height, std::vector<KlotskiRect>& other_rects) {
        KlotskiRect rect0 = *this;
        KlotskiRect rect1 = *this;
        KlotskiRect rect2 = *this;
        KlotskiRect rect3 = *this; 
        
        std::vector<KlotskiRect> result;

        if (rect0.executeAction(0, width, height, other_rects)) {
            result.push_back(rect0);
        }
        if (rect1.executeAction(1, width, height, other_rects)) {
            if (!(rect1 == rect0)) result.push_back(rect1);
        }
        if (rect2.executeAction(2, width, height, other_rects)) {
            result.push_back(rect2);
        }
        if (rect3.executeAction(3, width, height, other_rects)) {
            if (!(rect3 == rect2)) result.push_back(rect3);
        }

        return result;
    }    
};


struct KlotskiBoard {
    int width;
    int height;
    std::vector<KlotskiRect> blocks;


    //Creates empty klotski board
    KlotskiBoard(int width, int height) : width(width), height(height) {}

    //Adds block to rect ONLY if it is valid (not colliding with border or other rects). Return false if operation is unsuccessful
    bool addRect(KlotskiRect block) {
        if (block.checkCollisions(width, height, blocks)) return false;
        blocks.push_back(block);
        return true;
    }


    /*
    An action is composed of:
        the block you want to move
        0, 1, 2, 3 representing which way to move it (automatically truncates if block movement type is restricted)
    */
    //Returns true only if action is successful.
    bool executeAction(int blockID, int action) {
        assert(blockID >= 0 && blockID < blocks.size());
        return blocks[blockID].executeAction(action, width, height, blocks);
    }

    bool probeAction(int blockID, int action) {
        assert(blockID >= 0 && blockID < blocks.size());
        return blocks[blockID].probeAction(action, width, height, blocks);
    }

    
    /*
    For each block, we can generate what possible new block states are available
        1. Iterate through all possible actions
        2. Create a new whole Klotski board for each possible action taken.
            - If the Klotski board == the current board, the action failed and reverted to previous; do not add
    */
    std::vector<KlotskiBoard> calculateNewStates() {
        std::vector<KlotskiBoard> result;

        for (int blockID = 0; blockID < blocks.size(); blockID++) {
            for (int action = 0; action < 4; action++) {
                if (probeAction(blockID, action)) {
                    KlotskiBoard copy = *this;
                    copy.executeAction(blockID, action);
                    result.push_back(copy);
                }
            }
        }

        return result;
    }

    //*
    bool operator==(const KlotskiBoard& other) const {
        if (width != other.width || height != other.height) return false;    
        if (blocks.size() != other.blocks.size()) return false;

        for (size_t i = 0; i < blocks.size(); i++) {
            if (!(blocks[i] == other.blocks[i])) return false;
        }
        return true;
    }
    //*/
    
    /*
    //An equality operator between two std::vector<KlotskiRect>
    //If every rect in A can be mapped to exactly one equivilent rect in B and vice versa, both boards are equivilent.
    bool operator==(const KlotskiBoard& other) const {
        // 1. Basic dimension check
        if (width != other.width || height != other.height) {
            return false;
        }

        // 2. Count check
        if (blocks.size() != other.blocks.size()) {
            return false;
        }

        // 3. Deep comparison
        // We need to ensure every block in 'this' exists in 'other'
        // This handles cases where the blocks might be in a different order in the vector
        std::vector<bool> matched(other.blocks.size(), false);

        for (const auto& b1 : blocks) {
            bool foundMatch = false;
            for (size_t i = 0; i < other.blocks.size(); ++i) {
                // Use the KlotskiRect::operator== you defined
                // Note: You may need to mark your KlotskiRect operator== as const
                if (!matched[i] && const_cast<KlotskiRect&>(b1) == const_cast<KlotskiRect&>(other.blocks[i])) {
                    matched[i] = true;
                    foundMatch = true;
                    break;
                }
            }
            if (!foundMatch) return false;
        }

        return true;
    }
    //*/   


    std::string serialize() const {
        std::string result;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool contains = false;
                for (int blockID = 0; blockID < (int)blocks.size(); blockID++) {
                    if (blocks[blockID].containsPoint(x, y)) {
                        result += std::to_string(blockID);
                        contains = true;
                        break;
                    }
                }
                if (!contains) {
                    result += '_';
                }
                result += '\t';
            }
            result += '\n';
        }
        return result;
    }

    void print() {
        std::cout << serialize();
    }

};




void searchKlotskiStateSpace(std::vector<KlotskiBoard>& board_space, std::vector<std::pair<size_t, size_t>>& edges, int boardID = 0) {
    if (boardID % 300 == 0) std::cout << boardID << std::endl;
    //std::cout << ':' << std::endl;
    //board_space[boardID].print();


    assert(board_space.size() > boardID);
    /*
    for (int i = 1; checkValidity && i<state_space.size(); i++) {
        assert(state_space[i-1].width == state_space[i].width 
            && state_space[i-1].height == state_space[i].height
            && state_space[i-1].blocks.size() == state_space[i].blocks.size()
        );
    }
    */

    //Generating new potential states
    std::vector<KlotskiBoard> new_states = board_space[boardID].calculateNewStates();

    //Prune out duplicates and add to board space
    int initial_size = board_space.size();
    for (KlotskiBoard& new_board : new_states) {
        bool found = false;
        for (int other_id = 0; other_id < board_space.size(); other_id++) {
            if (new_board == board_space[other_id]) {
                if (!unordered_pair_exists(edges, boardID, other_id)) edges.emplace_back(boardID, other_id);
                found = true;
            }
        }
        if (!found) {
            //if (!unordered_pair_exists(edges, boardID, board_space.size()))
            edges.emplace_back(boardID, board_space.size());
            board_space.push_back(new_board);
        }
    }

    for (int i = initial_size; i<board_space.size(); i++) {
        searchKlotskiStateSpace(board_space, edges, i);
    }
}


StateSpace generate_klotski_ss(KlotskiBoard& initial_board, std::vector<std::string>& serialized_states) {
    StateSpace ss;

    std::vector<KlotskiBoard> board_space;
    board_space.push_back(initial_board);

    searchKlotskiStateSpace(board_space, ss.edges);

    for (KlotskiBoard& board : board_space) serialized_states.push_back(board.serialize());
    
    ss.NUM_NODES = board_space.size();

    return ss;
}


