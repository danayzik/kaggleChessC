#include "../include/move_sorters.h"
#include "../include/utilities.h"
#include <algorithm>
using namespace sorters;
using namespace bot_utils;
using namespace std;


int mvvLvaScore(const Board& board, const Move& move){
    Square from = move.from();
    Square to = move.to();
    int attacker = static_cast<int>(board.at(from).type());
    int victim = static_cast<int>(board.at(to).type());
    return victim * 10 - attacker;
}


void BasicMoveSorter::sortMovelist(Board& board, Movelist& moves) {
    auto comparator = [&board](const Move &a, const Move &b) {

        bool leftIsCheckMove = isCheckMove(board, a);
        bool rightIsCheckMove = isCheckMove(board, b);
        if (leftIsCheckMove && !rightIsCheckMove) return true;
        if (!leftIsCheckMove && rightIsCheckMove) return false;
        bool leftIsCapture = board.isCapture(a);
        bool rightIsCapture = board.isCapture(b);
        if(leftIsCapture && rightIsCapture){
            return mvvLvaScore(board, a) > mvvLvaScore(board, b);
        }
        if(leftIsCapture){
            return true;
        }
        if(rightIsCapture){
            return false;
        }
        return a.from() < b.from();
    };
    sort(moves.begin(), moves.end(), comparator);
}

BasicMoveSorter::BasicMoveSorter() {}