
#ifndef CHESSBOT_SORTERS_H
#define CHESSBOT_SORTERS_H
#include "chess.hpp"

using namespace chess;
using namespace std;

namespace sorters {
    class BasicMoveSorter {


    public:
        BasicMoveSorter();
        void sortMovelist(Board& board, Movelist& moves);

    };




}

#endif
