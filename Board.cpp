#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "Random.cpp"
#include "Board.hpp"

static const double probOf2 = 0.875;

template<int Row, int Col>
void Board<Row, Col>:: updateMaxTile() {
    for(auto & row : board) {
        for(auto & val : row)
            maxTile = max(maxTile, val);
    }
}

template<int Row, int Col>
void Board<Row, Col>:: updateRank() {
    if(playable() == Direction::NOT_VALID)
        rank = 0;
    else {
        double monotone = 0;
        double difference = 0;
        double space = 0;

        // Rank 1 : monotone sequence
        double bestMonotone = (Row-1)*Col + Row*(Col-1);
        for(int j=Col-1; j>=0; --j) {
            for(int i=Row-2; i>=0; --i)
                monotone += (board[i][j] >= board[i-1][j]);
        }
        for(int i=Row-1; i>=0; --i) {
            for(int j=1; j<Col; ++j)
                monotone += (board[i][j] >= board[i][j-1]);
        }
        monotone /= bestMonotone;

        // Rank 2 : difference
        double maxDifference = Row*Col*maxTile;
        for(int i=0; i<Row; ++i) {
            for(int j=0; j<Col; ++j) {
                if(board[i][j] == 0)
                    continue;
                
                for(int ii=i+1; ii<Row; ++ii) {
                    if(board[ii][j] > 0) {
                        difference += abs(board[ii][j] - board[i][j]);
                        break;
                    }
                }

                for(int jj=j+1; jj<Col; ++jj) {
                    if(board[i][jj] > 0) {
                        difference += abs(board[i][jj] - board[i][j]);
                        break;
                    }
                }
            }
        }
        if(difference > maxDifference) {
            std::cout << difference << ", " << maxDifference << std::endl;
            print(std::cout);
            std::cout << std::endl;
        }
        difference = 1.0 - difference/maxDifference;
        // std::cout << maxDifference << std::endl;

        //Rank 3 : Get as much as free space on the board
        for(auto & row : board) {
            for(auto & val : row)
                space += val==0;
        }
        space /= (Row*Col-1.0);

        rank = pow(monotone, exponents[0])*pow(difference, exponents[1])*pow(space, exponents[2]);
    }
}

template<int Row, int Col>
bool Board<Row, Col>:: horizontalFill(int row, int dir) {
    int start = (dir == -1) ? 1 : (Col-2);
    int end   = (dir == -1) ? Col : -1;
    bool flag = false;

    for(int c=start; c!=end; c-=dir) {
        int i=c;
        if(board[row][c] != 0) {
            while(i+dir!=-1 and i+dir!=Col and board[row][i+dir]==0) {
                swap(board[row][i], board[row][i+dir]);
                i += dir;
                flag = true;
            }
        }
    }

    return flag;
}

template<int Row, int Col>
bool Board<Row, Col>:: horizontalSwipe(int row, int dir) {
    int start = (dir == -1) ? 1 : (Col-2);
    int end   = (dir == -1) ? Col : -1;
    bool flag = false;

    flag |= horizontalFill(row, dir);

    for(int c=start; c!=end; c-=dir) {
        if(board[row][c] == board[row][c+dir]) {
            flag |= board[row][c]>0;
            board[row][c] = 0;
            board[row][c+dir] <<= 1;
        }
    }

    flag |= horizontalFill(row, dir);

    return flag;
}

template<int Row, int Col>
bool Board<Row, Col>:: verticalFill(int col, int dir) {
    int start = (dir == -1) ? 1 : (Row-2);
    int end   = (dir == -1) ? Row : -1;
    bool flag = false;

    for(int r=start; r!=end; r-=dir) {
        int i=r;
        if(board[r][col] != 0) {
            while(i+dir!=-1 and i+dir!=Row and board[i+dir][col]==0) {
                swap(board[i][col], board[i+dir][col]);
                i += dir;
                flag = true;
            }
        }
    }

    return flag;
}

template<int Row, int Col>
bool Board<Row, Col>:: verticalSwipe(int col, int dir) {
    int start = (dir == -1) ? 1 : (Row-2);
    int end   = (dir == -1) ? Row : -1;
    bool flag = false;

    flag |= verticalFill(col, dir);

    for(int r=start; r!=end; r-=dir) {
        if(board[r][col] == board[r+dir][col]) {
            flag |= board[r][col]>0;
            board[r][col] = 0;
            board[r+dir][col] <<= 1;
        }
    }

    flag |= verticalFill(col, dir);

    return flag;
}

template<int Row, int Col>
int Board<Row, Col>:: generateTile() {
    std::vector<std::pair<int, int>> freePos;
    freePos.reserve(Row * Col);

    for(int i=0; i<Row; ++i) {
        for(int j=0; j<Col; ++j) {
            if(board[i][j] == 0)
                freePos.emplace_back(i, j);
        }
    }

    auto pos = freePos[random_ab(0, freePos.size())];
    return board[pos.first][pos.second] = (random8() <= probOf2) ? 2 : 4;
}

template<int Row, int Col>
std::pair<Direction, double> Board<Row, Col>:: recBestMove(int K) {
    if(K == 0)
        return {playable(), (playable() == Direction::NOT_VALID ? 0 : rank)};
    
    Direction bestDir = Direction::NOT_VALID;
    double bestRank = 0;
    double expectedRank = 0;
    for(int i=3; i>=0; --i) {
        Direction dir = m[i];
        auto tmp = new Board<Row, Col>(*this);
        int gen = tmp->swipe(dir);

        if(gen != 0) {
            auto deepMove = tmp->recBestMove(K-1);

            if(deepMove.first != Direction::NOT_VALID) {
                double p = (gen == 2 ? probOf2 : (1.0-probOf2));
                double recRank = p * deepMove.second;
                expectedRank += recRank;

                if(bestRank <= recRank) {
                    bestRank = recRank;
                    bestDir = dir;
                }
            }
        }

        delete tmp;
    }

    return {bestDir, expectedRank};
}

/* --------------------------------------------------------- P U B L I C ---------------------------------------------------------- */
template<int Row, int Col>
Board<Row, Col>:: Board() : nOfMoves(0), maxTile(0), rank(0.0) {
    for(auto & row : board) {
        for(auto & val : row)
            val = 0;
    }

    int nOfRandom = max(1, Row * Col / 8);

    for(int i=0, r, c; i<nOfRandom; ++i) {
        r = random_ab(0, Row);
        c = random_ab(0, Col);

        board[r][c] = (random8() <= probOf2 ? 2 : 4);
        maxTile = max(maxTile, board[r][c]);
    }
}

template<int Row, int Col>
Board<Row, Col>:: Board(Board<Row, Col> & other) : nOfMoves(other.nOfMoves), maxTile(other.maxTile), rank(other.rank), board(other.board) {
    // Empty
}

template<int Row, int Col>
void Board<Row, Col>:: print(ostream & out) {
    for(auto & row : board) {
        for(auto & val : row)
            out << val << " ";
        out << std::endl;
    }
}

template<int Row, int Col>
bool Board<Row, Col>:: autoSwipe(Direction direction) {
    bool flag = false;

    switch(direction) {
        case Direction::UP:
            for(int c=0; c<Col; ++c)
                flag |= verticalSwipe(c, -1);
            break;
        
        case Direction::RIGHT:
            for(int r=0; r<Row; ++r)
                flag |= horizontalSwipe(r, +1);
            break;
        
        case Direction::DOWN:
            for(int c=0; c<Col; ++c)
                flag |= verticalSwipe(c, +1);
            break;
        
        case Direction::LEFT:
            for(int r=0; r<Row; ++r)
                flag |= horizontalSwipe(r, -1);
            break;
        
        default:
            std::cerr << "Swipe direction not valid!\n";
            break;
    }

    updateRank();

    return flag;
}

template<int Row, int Col>
int Board<Row, Col>:: swipe(Direction direction) {
    bool flag = autoSwipe(direction);
    int generated = 0;

    if(flag) {
        ++nOfMoves;
        generated = generateTile();
        updateMaxTile();
    }
    
    return generated;
}

template<int Row, int Col>
int Board<Row, Col>:: getLeader() {
    return maxTile;
}

template<int Row, int Col>
int Board<Row, Col>:: getLife() {
    return nOfMoves;
}

template<int Row, int Col>
double Board<Row, Col>:: getRank() {
    return rank;
}

template<int Row, int Col>
Direction Board<Row, Col>:: bestMove(int DEPTH) {
    Board<Row, Col> *cpy = new Board<Row, Col>(*this);
    Direction ans = cpy->recBestMove(DEPTH).first;
    delete cpy;
    return (ans == Direction::NOT_VALID ? playable() : ans);
}


template<int Row, int Col>
Direction Board<Row, Col>:: playable() {
    bool flag = false;
    Direction _move = Direction::NOT_VALID;

    for(int r=0; r<Row and not flag; ++r) {
        for(int c=0; c<Col and not flag; ++c) {
            if(r > 0) {
                flag = (board[r][c] == board[r-1][c]) or (board[r][c] != 0 and board[r-1][c] == 0);
                if(flag) {
                    _move = Direction::UP;
                    continue;
                }
            }
            if(r < Row-1) {
                flag = (board[r][c] == board[r+1][c]) or (board[r][c] != 0 and board[r+1][c] == 0);
                if(flag) {
                    _move = Direction::DOWN;
                    continue;
                }
            }
            if(c > 0) {
                flag = (board[r][c] == board[r][c-1]) or (board[r][c] != 0 and board[r][c-1] == 0);
                if(flag) {
                    _move = Direction::LEFT;
                    continue;
                }
            }
            if(c < Col-1) {
                flag = (board[r][c] == board[r][c+1]) or (board[r][c] != 0 and board[r][c+1] == 0);
                if(flag) {
                    _move = Direction::RIGHT;
                    continue;
                }
            }
        }
    }

    return _move;
}
