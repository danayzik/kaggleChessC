
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
        Move pv;
        [[nodiscard]] static inline int16_t mvvLvaScore(const Board& board, const Move& move) {
            Square from = move.from();
            Square to = move.to();
            int16_t attackerType = static_cast<int16_t>(board.at(from).type());
            int16_t victimType = static_cast<int16_t>(board.at(to).type());

            int16_t attackerValue = materialValue[attackerType];
            int16_t victimValue = materialValue[victimType];
            if (victimValue >= attackerValue) {
                return (victimValue * 10) - attackerValue;
            } else {
                return -(attackerValue - victimValue);
            }
        }
    public:
        virtual inline void setHistoryTable(int (*table)[64][64]){};
        virtual inline void setPv(Move& move) {};
        virtual void sortMovelist(Board& board, Movelist& moves) const = 0;
        virtual ~MoveSorter()= default;
        virtual inline void setKillerMoves(Move (*table)[40][2]){};
        virtual inline void setDepth(int depth){};
        virtual inline void setTT(transpositions::TranspositionTable* table) {};

    };


    class BasicMoveSorter : public MoveSorter{
    public:
        BasicMoveSorter() = default;
        void sortMovelist(Board& board, Movelist& moves) const override;
    };

    class PvMoveSorter : public MoveSorter{
    public:

        PvMoveSorter() = default;
        void sortMovelist(Board& board, Movelist& moves) const override;
        inline void setPv(Move& move) override {pv = move;};
    };

    class PvHistoryMoveSorter : public MoveSorter{
    protected:
        int (*historyTable)[64][64] = nullptr;
    public:
        inline void setHistoryTable(int (*table)[64][64]) override {historyTable = table;}
        PvHistoryMoveSorter() = default;
        void sortMovelist(Board& board, Movelist& moves) const override;
        inline void setPv(Move& move) override {pv = move;};
    };

    class PvHistoryKillerMoveSorter : public MoveSorter{
    protected:
        int (*historyTable)[64][64] = nullptr;
        Move (*killerMoves)[40][2] = nullptr;
        int currDepth = 0;
    public:
        inline void setHistoryTable(int (*table)[64][64]) override {historyTable = table;}
        void sortMovelist(Board& board, Movelist& moves) const override;
        inline void setPv(Move& move) override {pv = move;};
        inline void setKillerMoves(Move (*table)[40][2]) override{killerMoves = table;}
        PvHistoryKillerMoveSorter() = default;
        inline void setDepth(int depth) override{currDepth =depth;}


    };

    class PvHistoryKillerTTMoveSorter : public MoveSorter{
    protected:
        int (*historyTable)[64][64] = nullptr;
        Move (*killerMoves)[40][2] = nullptr;
        transpositions::TranspositionTable* TT = nullptr;
        int currDepth = 0;
    public:
        inline void setHistoryTable(int (*table)[64][64]) override {historyTable = table;}
        void sortMovelist(Board& board, Movelist& moves) const override;
        inline void setPv(Move& move) override {pv = move;};
        inline void setKillerMoves(Move (*table)[40][2]) override{killerMoves = table;}
        inline void setTT(transpositions::TranspositionTable* table)override {TT = table;}
        PvHistoryKillerTTMoveSorter() = default;
        inline void setDepth(int depth) override{currDepth =depth;}


    };





}

#endif
