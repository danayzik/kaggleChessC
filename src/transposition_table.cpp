#include "../include/transposition_table.h"

using namespace transpositions;

TranspositionTable::TranspositionTable(size_t evictionThreshold, size_t resetSize)
        : evictionThreshold(evictionThreshold), resetSize(resetSize) {}

void TranspositionTable::store(const uintptr_t key, const TTEntry& entry) {
    if (!hasKey(key)) size++;
    table[key] = entry;
    this->clean();
}

std::pair<int, Move> TranspositionTable::fetch(const uintptr_t key) {
    TTEntry entry = table[key];
    return {entry.eval, entry.bestMove};
}

bool TranspositionTable::hasKey(const uintptr_t key) {
    return table.count(key);
}

void TranspositionTable::clean() {
    if (size >= evictionThreshold) {
        evictEntries();
    }
}

void TranspositionTable::evictEntries() {
    std::vector<std::pair<uint64_t, TTEntry>> vec(table.begin(), table.end());
    auto comparator = [](const std::pair<uint64_t, TTEntry>& a, const std::pair<uint64_t, TTEntry>& b) {
        const TTEntry& entryA = a.second;
        const TTEntry& entryB = b.second;
        uint32_t diffFullMoves = (entryA.fullMoves > entryB.fullMoves)
                                 ? entryA.fullMoves - entryB.fullMoves
                                 : entryB.fullMoves - entryA.fullMoves;
        if (diffFullMoves > 5) {
            return entryA.fullMoves < entryB.fullMoves;
        } else {
            return entryA.depth < entryB.depth;
        }
    };

    std::sort(vec.begin(), vec.end(), comparator);
    size_t numToEvict = size - resetSize;
    for (size_t i = 0; i < numToEvict && vec.size() > i; ++i) {
        table.erase(vec[vec.size() - 1 - i].first);
    }
    size = resetSize;
}
