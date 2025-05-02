
#ifndef CHESSBOT_SORTERS_H
#define CHESSBOT_SORTERS_H
#include "chess.hpp"
#include "transposition_table.h"
#include "utilities.h"
using namespace chess;
using namespace std;
using namespace bot_utils;

namespace sorters {
    class MoveSorter {
    protected:
        [[nodiscard]] static inline int16_t mvvLvaScore(const Board& board, const Move& move) {
            Square from = move.from();
            Square to = move.to();
            int16_t attackerType = static_cast<int16_t>(board.at(from).type());
            int16_t victimType = static_cast<int16_t>(board.at(to).type());

            int16_t attackerValue = materialValue[attackerType];
            int16_t victimValue = materialValue[victimType];
            if (victimValue > attackerValue) {
                return (victimValue * 10) - attackerValue;
            } else {
                return -(attackerValue - victimValue);
            }
        }
    public:
        virtual inline void setHistoryTable(int (*table)[2][64][64]){};
        virtual inline Move selectNextMove(Movelist& moves, int startIndex) const{return {};};
        virtual inline void setMoveScores(const Board& board, Movelist& moves) const{};
        virtual inline void setPv(Move& move) {};
        virtual void sortMovelist(Board& board, Movelist& moves) const = 0;
        virtual ~MoveSorter()= default;
        virtual inline void setKillerMoves(Move (*table)[MAX_DEPTH][2]){};
        virtual inline void setDepths(int currDepth, int currPlyFromRoot){};
        virtual inline void setPvLine(Move* pvLine) {};
        virtual inline void setMaxPvDepth(int d){};
        virtual void sortCaptures(const Board& board, Movelist& moves) const{};

    };


    class negaMaxSorter : public MoveSorter{
    protected:
        int maxValidPvDepth = 0;
        int plyFromRoot = 0;
        int depth = 0;
        int (*historyTable)[2][64][64] = nullptr;
        Move (*killerMoves)[MAX_DEPTH][2] = nullptr;
        Move* pv = nullptr;
    public:
        void sortMovelist(Board& board, Movelist& moves) const override;
        inline void setHistoryTable(int (*table)[2][64][64]) override {historyTable = table;}
        inline void setMaxPvDepth(int d) override { maxValidPvDepth = d;}
        inline void setPvLine(Move* pvLine) override{pv = pvLine;};
        inline void setKillerMoves(Move (*table)[MAX_DEPTH][2]) override{killerMoves = table;}
        negaMaxSorter() = default;
        inline void setDepths(int currDepth, int currPlyFromRoot) override{ plyFromRoot = currPlyFromRoot; depth = currDepth;}
        void sortCaptures(const Board& board, Movelist& moves) const override;
    };


}

#endif
