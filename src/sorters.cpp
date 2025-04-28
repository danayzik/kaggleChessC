#include "../include/move_sorters.h"
#include "../include/utilities.h"
#include <algorithm>
#include <limits>

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
    for (auto& move : moves) {
        int16_t score = -20000;
        if(move==pv){
            move.setScore(INT16_MAX);
            continue;
        }

        if (isCheckMove(board, move)) {
            score += 15000;
        }

        if (board.isCapture(move)) {
            score += mvvLvaScore(board, move)*100 + 400;
        }


        score += (*historyTable)[move.from().index()][move.to().index()];
        move.setScore(score);

    }

    auto comparator = [](const Move& a, const Move& b) {
        return a.score() > b.score();
    };

    std::sort(moves.begin(), moves.end(), comparator);
}



void PvHistoryKillerMoveSorter::sortMovelist(Board &board, Movelist& moves) const {
    for (auto& move : moves) {
        int16_t score = -20000;
        if(move==pv){
            move.setScore(INT16_MAX);
            continue;
        }

        if (isCheckMove(board, move)) {
            score += 15000;
        }

        if (board.isCapture(move)) {
            score += mvvLvaScore(board, move)*100 + 400;
        }

        if (move == (*killerMoves)[currDepth][0] || move == (*killerMoves)[currDepth][1]) {
            score += 300;
        }

        score += (*historyTable)[move.from().index()][move.to().index()];
        move.setScore(score);

    }

    auto comparator = [](const Move& a, const Move& b) {
        return a.score() > b.score();
    };

    std::sort(moves.begin(), moves.end(), comparator);
}

void PvHistoryKillerTTMoveSorter::sortMovelist(Board &board, Movelist& moves) const {

    for (auto& move : moves) {
        int16_t score = -20000;
        if(move==pv){
            move.setScore(INT16_MAX);
            continue;
        }
        if(TT->isHashMove(move, board)){
            score += 30000;
        }

        if (isCheckMove(board, move)) {
            score += 15000;
        }

        if (board.isCapture(move)) {
            score += mvvLvaScore(board, move)*100 + 400;
        }

        if (move == (*killerMoves)[currDepth][0] || move == (*killerMoves)[currDepth][1]) {
            score += 300;
        }

        score += (*historyTable)[move.from().index()][move.to().index()];
        move.setScore(score);

    }

    auto comparator = [](const Move& a, const Move& b) {
        return a.score() > b.score();
    };

    std::sort(moves.begin(), moves.end(), comparator);
}




