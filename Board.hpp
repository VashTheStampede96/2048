#ifndef _2048_BOARD_HPP_
#define _2048_BOARD_HPP_

enum Direction {LEFT, UP, DOWN, RIGHT, NOT_VALID};
Direction m[5] = {Direction::RIGHT, Direction::UP, Direction::DOWN, Direction::LEFT, Direction::NOT_VALID};
vector<double> exponents = {1.8, 0.6, 1.4};

template<int Row, int Col>
class Board {
private:
    int nOfMoves, maxTile;
    double rank;
    std::array<std::array<int, Col>, Row> board;

    void updateMaxTile();
    void updateRank();

    bool horizontalSwipe(int, int);
    bool horizontalFill(int, int);
    bool verticalFill(int, int);
    bool verticalSwipe(int, int);    

    int generateTile();

    std::pair<Direction, double> recBestMove(int);

public:
    Board();
    Board(Board<Row, Col>&);

    void print(ostream&);

    bool autoSwipe(Direction);
    
    int getLeader();
    int getLife();
    int swipe(Direction);
    
    double getRank();
    
    Direction bestMove(int);
    Direction playable();
    
};

#endif