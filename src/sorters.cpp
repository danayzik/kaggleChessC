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



void PvHistoryKillerMoveSorter::sortMovelist(Board &board, Movelist& moves) const {
    std::unordered_map<uint16_t, int> moveScores;

    for (const auto& move : moves) {
        int score = 0;
        if(move==pv){
            score += 100000;
        }

        if (isCheckMove(board, move)) {
            score += 10000;
        }

        if (board.isCapture(move)) {
            score += mvvLvaScore(board, move)*1000;
        }

        if (move == (*killerMoves)[currDepth][0] || move == (*killerMoves)[currDepth][1]) {
            score += 100;
        }

        score += (*historyTable)[move.from().index()][move.to().index()];

        moveScores[move.move()] = score;
    }

    auto comparator = [&moveScores](const Move& a, const Move& b) {
        return moveScores[a.move()] > moveScores[b.move()];
    };

    std::sort(moves.begin(), moves.end(), comparator);
}

void PvHistoryKillerTTMoveSorter::sortMovelist(Board &board, Movelist& moves) const {
    std::unordered_map<uint16_t, int> moveScores;

    for (const auto& move : moves) {
        int score = 0;
        if(move==pv){
            score += 1000000;
        }
        if(TT->isHashMove(move, board)){
            score += 100000;
        }

        if (isCheckMove(board, move)) {
            score += 10000;
        }

        if (board.isCapture(move)) {
            score += mvvLvaScore(board, move)*1000 + 1000;
        }

        if (move == (*killerMoves)[currDepth][0] || move == (*killerMoves)[currDepth][1]) {
            score += 400;
        }

        score += (*historyTable)[move.from().index()][move.to().index()];

        moveScores[move.move()] = score;
    }

    auto comparator = [&moveScores](const Move& a, const Move& b) {
        return moveScores[a.move()] > moveScores[b.move()];
    };

    std::sort(moves.begin(), moves.end(), comparator);
}




