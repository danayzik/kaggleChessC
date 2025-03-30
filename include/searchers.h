
#ifndef CHESSBOT_SEARCHERS_H
#define CHESSBOT_SEARCHERS_H
#include "chess.hpp"
#include <map>
#include "evaluator.h"
#include "transposition_table.h"
#include "move_sorters.h"
using namespace sorters;
using namespace chess;
using namespace std;
using namespace evaluation;
using namespace transpositions;

namespace searchers {
    class Searcher {
    protected:

        Evaluator* evaluator;
        bool isWhite;
        Searcher(Evaluator* evaluator, bool isWhite)
                : evaluator(evaluator), isWhite(isWhite) {}

    public:
        virtual Move getMove(Board &board, int msTimeLimit) = 0;

    };

    class BasicSearcher : public Searcher {
    protected:
        int maxDepth = 4;
        BasicMoveSorter sorter;
        pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool is_maximizing);

    public:
        BasicSearcher(Evaluator* evaluator, bool isWhite);
        Move getMove(Board &board, int msTimeLimit) override;
    };


}

#endif
