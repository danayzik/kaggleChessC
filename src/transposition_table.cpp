#include "../include/transposition_table.h"

using namespace transpositions;

bool TranspositionTable::isHashedPosition(uint64_t key, int depth) const {
    const TTEntry& entry = table[key % size];
    return entry.key == key && entry.depth>= depth;
}

bool TranspositionTable::probe(uint64_t key, int depth, transpositions::TTEntry &outEntry) const {
    const TTEntry& entry = table[key % size];
    if (entry.key == key && entry.depth>= depth) {
        outEntry = entry;
        return true;
    }
    return false;
}

void TranspositionTable::store(uint64_t key, int eval, Move& m, int depth, transpositions::EntryType type,
                               uint32_t fullMoves) {
    TTEntry& entry = table[key % size];
    if (((fullMoves >= entry.fullMoveClock) && (fullMoves - entry.fullMoveClock >= 2))|| entry.key == 0 || depth > entry.depth) {
        entry = {key, eval, m, depth, type, fullMoves};
    }
}