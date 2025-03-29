
#ifndef CHESSBOT_EVALUATOR_H
#define CHESSBOT_EVALUATOR_H
#include "chess.hpp"
using namespace chess;

class Evaluator {
private:

    bool gameOver;
    bool isDraw;
    Color winner;

public:
    int getEval(const Board& board);

};





#endif //CHESSBOT_EVALUATOR_H
