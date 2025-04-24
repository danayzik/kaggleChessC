
#ifndef CHESSBOT_SORTERS_H
#define CHESSBOT_SORTERS_H
#include "chess.hpp"

using namespace chess;
using namespace std;

namespace sorters {
    class MoveSorter {
    protected:
        Move pv;
        [[nodiscard]] static inline int mvvLvaScore(const Board& board, const Move& move) {
            Square from = move.from();
            Square to = move.to();
            int attacker = static_cast<int>(board.at(from).type());
            int victim = static_cast<int>(board.at(to).type());
            return victim * 10 - attacker;
        }
    public:
        virtual inline void setHistoryTable(int (*table)[64][64]){};
        virtual inline void setPv(Move& move) {};
        virtual void sortMovelist(Board& board, Movelist& moves) const = 0;
        virtual ~MoveSorter()= default;
        virtual inline void setKillerMoves(Move (*table)[40][2]){};
        virtual inline void setDepth(int depth){};
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





}

#endif
