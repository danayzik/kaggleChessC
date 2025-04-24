#include "../include/move_sorters.h"
#include "../include/utilities.h"
#include <algorithm>
using namespace sorters;
using namespace bot_utils;
using namespace std;





void BasicMoveSorter::sortMovelist(Board& board, Movelist& moves) const {
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

void PvMoveSorter::sortMovelist(Board& board, Movelist& moves) const {
    auto comparator = [&board, this](const Move &a, const Move &b) {
        if(a == pv)return true;
        if(b == pv)return false;
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


void PvHistoryMoveSorter::sortMovelist(chess::Board &board, chess::Movelist &moves) const {
    auto comparator = [&board, this](const Move &a, const Move &b) {
        if(a == pv)return true;
        if(b == pv)return false;
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

        int historyScoreA = (*historyTable)[a.from().index()][a.to().index()];
        int historyScoreB = (*historyTable)[b.from().index()][b.to().index()];
        if (historyScoreA != historyScoreB) {
            return historyScoreA > historyScoreB;
        }
        return a.from() < b.from();
    };
    sort(moves.begin(), moves.end(), comparator);
}

void PvHistoryKillerMoveSorter::sortMovelist(chess::Board &board, chess::Movelist &moves) const {
    auto comparator = [&board, this](const Move &a, const Move &b) {
        if(a == pv)return true;
        if(b == pv)return false;
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
        auto isKiller = [this](const Move& m) {
            return m == (*killerMoves)[currDepth][0] || m == (*killerMoves)[currDepth][1];
        };
        bool aIsKiller = isKiller(a);
        bool bIsKiller = isKiller(b);
        if (aIsKiller && !bIsKiller) return true;
        if (!aIsKiller && bIsKiller) return false;

        int historyScoreA = (*historyTable)[a.from().index()][a.to().index()];
        int historyScoreB = (*historyTable)[b.from().index()][b.to().index()];
        if (historyScoreA != historyScoreB) {
            return historyScoreA > historyScoreB;
        }
        return a.from() < b.from();
    };
    sort(moves.begin(), moves.end(), comparator);
}