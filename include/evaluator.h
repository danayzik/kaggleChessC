
#ifndef CHESSBOT_EVALUATOR_H
#define CHESSBOT_EVALUATOR_H
#include "chess.hpp"
#include <map>
using namespace chess;
using namespace std;

namespace evaluation {
    class Evaluator {
    protected:

        bool gameOver;
        bool isDraw;
        Color winner;


    public:
        virtual int getEval(const Board &board) = 0;

        virtual void setGameOver(bool isDraw, Color winner) = 0;

        virtual void setGameOngoing() = 0;

    };

    class HeuristicEvaluator : public Evaluator {
    protected:
        array<int, 2> scores;
        array<int, 2> mg;
        array<int, 2> eg;
        int gamePhase;
        map<PieceType, int> attackValues;
        map<PieceType, int> defendValues;
        const int mobilityBonus = 4;
        const int halfOpenFileBonus = 25;
        const int fullyOpenFileBonus = 60;
        const int outPostBonus = 80;
        const int passedPawnBonus = 40;
        const int castlingRightsBonus = 10;
        Color workingColor;

        void setGameOver(bool isDraw, Color winner) override;

        void setGameOngoing() override;

        int endgameMateEval(const Board &board, int egPhase, int currEval);

        int evaluatePieces(const Board &board);

        void evaluateKnights(const Board &board);

        void evaluateKing(const Board &board);

        void evaluatePawns(const Board &board);

        void evaluateBlockedPieces(const Board &board);

        void addAttackDefendValues(const Board &board, Bitboard controlledSquares);

        void addRookOnOpenFile(const Board &board, Square rookSquare);

        void addOutpostBonus(const Board &board, Square knightSquare);

        void addMobilityBonus(const Board &board, Bitboard controlledSquares, bool isKing);

        void addPassedPawnBonus(const Board &board, Square pawnSquare);

        void incPestoValues(int colorIndex, int pc, int sqrIndex);
        void initEval();

    public:
        int getEval(const Board &board) override;

    };


}

#endif //CHESSBOT_EVALUATOR_H
