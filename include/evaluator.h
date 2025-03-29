
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
        virtual int getEval(const Board &board);

        virtual void resetParams();

        virtual void setGameOver(bool isDraw, Color winner);

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



        int endgameMateEval(const Board &board, int egPhase, int currEval);

        int evaluatePieces(const Board &board);

        void evaluateKnights(const Board &board, Color color);

        void evaluateKing(const Board &board, Color color);

        void evaluatePawns(const Board &board, Color color);

        void evaluateBlockedPieces(const Board &board, Color color);

        void addAttackDefendValues(const Board &board, Bitboard controlledSquares, Color color);

        void addRookOnOpenFile(const Board &board, Square rookSquare, Color color);

        void addOutpostBonus(const Board &board, Square knightSquare, Color color);

        void addMobilityBonus(const Board &board, Bitboard controlledSquares, Color color, bool isKing);

        void addPassedPawnBonus(const Board &board, Square pawnSquare, Color color);

        void incPestoValues(int colorIndex, int pc, int sqrIndex);
        void initEval();
    public:
        int getEval(const Board &board) override;

    };


}

#endif //CHESSBOT_EVALUATOR_H
