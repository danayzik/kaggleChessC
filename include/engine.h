
#ifndef CHESSBOT_ENGINE_H
#define CHESSBOT_ENGINE_H
#include "chess.hpp"
#include "evaluator.h"
using namespace chess;

class Engine {
private:
    Board board;
    Color myColor;
    bool isMaximizing;
    Evaluator evaluator;


public:
    Engine(Board board, Color color);
    void makeMove(const Move& move);
    int getEval();
    Move getMove();
};





#endif //CHESSBOT_ENGINE_H
