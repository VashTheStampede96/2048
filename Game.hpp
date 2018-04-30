#ifndef _2048_GAME_HPP_
#define _2048_GAME_HPP_

template<int Row, int Col>
class Game {
private:
    int depth, score;
    Board<Row, Col> *board;

public:
    Game(int d=1);
    ~Game();

    void run();

    int getScore();
    
    double autoPlay();

};

#endif