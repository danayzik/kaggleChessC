#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <vector>
#include <algorithm>
#include "chess.hpp"

using namespace chess;
namespace transpositions {
    enum EntryType { EXACT, LOWERBOUND, UPPERBOUND };
    struct TTEntry {
        uint64_t key;
        int eval;
        Move bestMove;
        EntryType entryType;
        int depth;
        uint32_t fullMoveClock;

        inline TTEntry(uint64_t key, int eval, Move m, int depth, EntryType type, uint32_t fullMoves)
                : key(key), eval(eval), bestMove(m), depth(depth), entryType(type), fullMoveClock(fullMoves){};
        inline TTEntry() = default;
    };

    class TranspositionTable {
    private:
        std::vector<TTEntry> table;
        size_t size;

    public:
        explicit TranspositionTable(size_t MB){
            size = (MB * 1024 * 1024) / sizeof(TTEntry);
            table.resize(size);}

        void store(uint64_t key, int eval, Move m, int depth, EntryType type, uint32_t fullMoves);


        bool probe(uint64_t key, int depth, TTEntry& outEntry) const;

        bool isHashMove(const Move& move, Board& board) const;

    };
}

#endif
