#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <unordered_map>
#include <vector>
#include <algorithm>
#include "chess.hpp"

using namespace chess;

struct TTEntry {
    int eval;
    Move bestMove;
    int depth;
    uint32_t fullMoves;

    explicit TTEntry(int e = 0, Move m = Move(), int d = 0, uint32_t f = 0)
            : eval(e), bestMove(m), depth(d), fullMoves(f) {}
};

class TranspositionTable {
private:
    std::unordered_map<uint64_t, TTEntry> table;
    size_t evictionThreshold;
    size_t resetSize;
    size_t size = 0;

    void evictEntries();

public:
    TranspositionTable(size_t evictionThreshold, size_t resetSize);

    void store(const uintptr_t key, const TTEntry& entry);
    std::pair<int, Move> fetch(const uintptr_t key);
    bool hasKey(const uintptr_t key);
    void clean();
};

#endif
