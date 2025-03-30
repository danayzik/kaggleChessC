
#ifndef CHESSBOT_ENGINE_H
#define CHESSBOT_ENGINE_H
#include "chess.hpp"
#include "evaluator.h"
#include "searchers.h"
using namespace searchers;
using namespace chess;
using namespace evaluation;


namespace engines {
    class Engine {
    private:
        Board board;
        Color myColor;
        bool isWhite;
        Evaluator* evaluator;
        Searcher* searcher;

    public:
        Engine(Board&& board, Color myColor, Evaluator* evaluator, Searcher* searcher);

        void makeMove(const Move &move);

        [[maybe_unused]] int getEval();

        Move getMove();

        const Board& getBoard();
    };

}



#endif
