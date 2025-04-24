
#ifndef CHESSBOT_SEARCHERS_H
#define CHESSBOT_SEARCHERS_H
#include "chess.hpp"
#include <map>
#include "evaluator.h"
#include <chrono>
#include <memory>
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
        bool stopSearch = false;
        int searchTimeLimit = 0;
        bool validSearchResult = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> searchStartTime;

        Evaluator* evaluator;
        bool isWhite;
        std::unique_ptr<MoveSorter> sorter;
        inline Searcher(Evaluator* evaluator, bool isWhite, std::unique_ptr<MoveSorter> _sorter)
                : evaluator(evaluator), isWhite(isWhite), sorter(std::move(_sorter)) {}

    public:
        virtual Move getMove(Board &board, int msTimeLimit) = 0;

    };

    class BasicSearcher : public Searcher {
    protected:
        int maxDepth = 4;
        pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing);

    public:
        BasicSearcher(Evaluator* evaluator, bool isWhite);
        Move getMove(Board &board, int msTimeLimit) override;

    };

    class BasicIterativeSearcher : public Searcher {
    protected:

        pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing);
        virtual pair<int , Move> iterativeDeepening(Board& board);

    public:
        BasicIterativeSearcher(Evaluator* evaluator, bool isWhite);
        Move getMove(Board &board, int msTimeLimit) override;
    };

    class IterativePvSearcher : public Searcher{
    protected:
        pair<int, Move> minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing, bool inRoot);
        pair<int, Move> iterativeDeepening(Board& board);

    public:
        IterativePvSearcher(Evaluator* evaluator, bool isWhite);
        Move getMove(Board &board, int msTimeLimit) override;

    };

    class IterativePvHistorySearcher : public Searcher{
    private:
        int historyTable[64][64] = {};
        inline void decayHistory() {
            for(auto & i : historyTable)
                for(int & j : i)
                    j /= 2;
        }
    protected:
        pair<int , Move> iterativeDeepening(Board& board);
        pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing, bool inRoot);
    public:
        IterativePvHistorySearcher(Evaluator* evaluator, bool isWhite);
        Move getMove(Board &board, int msTimeLimit) override;
    };

    class IterativePvHistoryKillerSearcher : public Searcher{
    private:
        int historyTable[64][64] = {};
        Move killerMoves[40][2] = {};
        inline void decayHistory() {
            for(auto & i : historyTable)
                for(int & j : i)
                    j /= 2;
        }
    protected:
        pair<int , Move> iterativeDeepening(Board& board);
        pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing, bool inRoot);
    public:
        IterativePvHistoryKillerSearcher(Evaluator* evaluator, bool isWhite);
        Move getMove(Board &board, int msTimeLimit) override;
    };
}

#endif
