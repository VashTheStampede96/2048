#include <iostream>
#include <chrono>
#include "Board.cpp"
#include "Game.hpp"

template<int Row, int Col>
Game<Row, Col>:: Game(int d) : depth(d), score(0) {
    auto t = std::chrono::system_clock::now().time_since_epoch();
    set_seed( std::chrono::duration<double>(t).count() );
    
    board = new Board<Row, Col>();
}

template<int Row, int Col>
Game<Row, Col>:: ~Game() {
    delete board;
}

template<int Row, int Col>
int Game<Row, Col>:: getScore() {
    return score;
}

template<int Row, int Col>
void Game<Row, Col>:: run() {        
    std::cout << "Welcome to 2048!\n";
    std::cout << "You are going to play the " << Row << "x" << Col << " version!\n";

    board->print();
    std::cout << std::endl;

    char c;
    while(board->playable()) {
        cin >> c;
        switch(c) {
            case 'l':
                board->swipe(Direction::LEFT);
                break;
            case 'r':
                board->swipe(Direction::RIGHT);
                break;
            case 'u':
                board->swipe(Direction::UP);
                break;
            case 'd':
                board->swipe(Direction::DOWN);
                break;
            default:
                break;
        }

        board->print();
        std::cout << std::endl;

        std::cout << "Rank = " << board->getRank() << "\n";
    }

    std::cout << "YOU LOSE, BABBEO!\n";
}

template<int Row, int Col>
double Game<Row, Col>:: autoPlay() {
    delete board;
    board = new Board<Row, Col>();

    while(board->playable() != Direction::NOT_VALID) {
        Direction bMove = board->bestMove(depth);
        board->swipe(bMove);
    }
    score = board->getLeader();

    return board->getLife();
}