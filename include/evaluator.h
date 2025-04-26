
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
        std::vector<PieceType> blockedPieces = {PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN};
        std::vector<Square> cornerSquares = {Square(0), Square(7), Square(63), Square(56)};
        std::vector<Square> centerSquares = {Square(27), Square(28), Square(35), Square(36)};
        const int mobilityBonus = 4;
        const int halfOpenFileBonus = 25;
        const int fullyOpenFileBonus = 60;
        Bitboard occ;
        const int outPostBonus = 80;
        const int passedPawnBonus = 40;
        const int castlingRightsBonus = 10;
        Color workingColor;
        bool workingColorIndex;

        static Bitboard getAttackSquaresForPiece(PieceType type, Square sq, Bitboard& occ);

        void setGameOver(bool isDraw, Color winner) override;

        void setGameOngoing() override;

        int endgameMateEval(const Board &board, int egPhase, int currEval);

        int evaluatePieces(const Board &board);

        void evaluateKnights(const Board &board);

        void evaluateKing(const Board &board);

        void evaluatePawns(const Board &board);

        void evaluateBlockedPieces(const Board &board);


        void addRookOnOpenFile(const Board &board, const Square& rookSquare);

        void addOutpostBonus(const Board &board, const Square& knightSquare);

        void addMobilityBonus(const Board &board, const Bitboard& controlledSquares, bool isKing);

        void addPassedPawnBonus(const Board &board, const Square& pawnSquare);

        inline void incPestoValues(int colorIndex, int pc, int sqrIndex);
        inline void initEval();

    public:
        int getEval(const Board &board) override;

    };




}

#endif //CHESSBOT_EVALUATOR_H
