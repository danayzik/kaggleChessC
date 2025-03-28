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
    std::unordered_map<uint64_t , TTEntry> table;
    size_t evictionThreshold;
    size_t resetSize;
    size_t size = 0;


public:
    TranspositionTable(size_t evictionThreshold, size_t resetSize)
            :  evictionThreshold(evictionThreshold), resetSize(resetSize) {}


    void store(const uintptr_t key, const TTEntry& entry) {
        if(!hasKey(key)) size++;
        table[key] = entry;
        this->clean();
    }

    // Assumes hasKey has been called and is true
    std::pair<int, Move> fetch(const uintptr_t key) {
        TTEntry entry = table[key];
        return {entry.eval, entry.bestMove};

    }

    bool hasKey(const uintptr_t key) {
        return table.count(key);
    }

    //Call this between searches
    void clean(){
        if (size >= evictionThreshold){
            evictEntries();
        }
    }

private:

    void evictEntries() {
        std::vector<std::pair<uint64_t , TTEntry>> vec(table.begin(), table.end());
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
};
