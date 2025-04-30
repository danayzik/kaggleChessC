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


        score += (*historyTable)[board.sideToMove()][move.from().index()][move.to().index()];
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

        if (move == (*killerMoves)[plyFromRoot][0] || move == (*killerMoves)[plyFromRoot][1]) {
            score += 300;
        }

        score += (*historyTable)[board.sideToMove()][move.from().index()][move.to().index()];
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
        setCheckAndHashMove(board, move, depth, TT);
        if(move.isHash){
            score += 30000;
        }

        if (move.isCheck) {
            score += 15000;
        }

        if (board.isCapture(move)) {
            score += mvvLvaScore(board, move)*100 + 400;
        }

        if (move == (*killerMoves)[plyFromRoot][0] || move == (*killerMoves)[plyFromRoot][1]) {
            score += 300;
        }

        score += (*historyTable)[board.sideToMove()][move.from().index()][move.to().index()];
        move.setScore(score);

    }

    auto comparator = [](const Move& a, const Move& b) {
        return a.score() > b.score();
    };

    std::sort(moves.begin(), moves.end(), comparator);
}

void negaMaxSorter::sortMovelist(Board &board, Movelist& moves) const {

    for (auto& move : moves) {
        if(pv[plyFromRoot] == move && plyFromRoot <= maxValidPvDepth){
            move.setScore(INT16_MAX);
            continue;
        }
        int16_t score = 0;
        int historyScore = (*historyTable)[board.sideToMove()][move.from().index()][move.to().index()];
        bool isKiller = move == (*killerMoves)[plyFromRoot][0] || move == (*killerMoves)[plyFromRoot][1];
        bool isHistory = historyScore != 0;
        bool isCapture = board.isCapture(move);
        if(isCapture){
            score += mvvLvaScore(board, move) + 500;
        }
        if(isKiller){
            score += 300;
        }
        if(isHistory){
            score += historyScore;
        }
        //Regular move
        if(!(isCapture || isKiller || isHistory)){
            score = INT16_MIN;
        }
        move.setScore(score);



    }

    auto comparator = [](const Move& a, const Move& b) {
        return a.score() > b.score();
    };

    std::sort(moves.begin(), moves.end(), comparator);
}

void negaMaxSorter::setMoveScores(const chess::Board &board, chess::Movelist &moves) const {
    for (auto& move : moves) {
        if(pv[plyFromRoot] == move && plyFromRoot <= maxValidPvDepth){
            move.setScore(INT16_MAX);
            continue;
        }
        int16_t score = 0;
        int historyScore = (*historyTable)[board.sideToMove()][move.from().index()][move.to().index()];
        bool isKiller = move == (*killerMoves)[plyFromRoot][0] || move == (*killerMoves)[plyFromRoot][1];
        bool isHistory = historyScore != 0;
        bool isCapture = board.isCapture(move);
        if(isCapture){
            score += mvvLvaScore(board, move) + 500;
        }
        if(isKiller){
            score += 300;
        }
        if(isHistory){
            score += historyScore;
        }
        //Regular move
        if(!(isCapture || isKiller || isHistory)){
            score = INT16_MIN;
        }
        move.setScore(score);


    }

}

Move negaMaxSorter::selectNextMove(chess::Movelist &moves, int startIndex) const {
    for(int i = startIndex + 1; i < moves.size(); i++){
        if(moves.at(i).score() > moves.at(startIndex).score()){
            std::swap(moves[i], moves[startIndex]);
        }
    }
    return moves.at(startIndex);
}

void negaMaxSorter::sortCaptures(const chess::Board &board, chess::Movelist &moves) const {
    for (auto& move : moves) {
        int16_t score = 0;
        score += mvvLvaScore(board, move) + 500;
        if(move == (*killerMoves)[plyFromRoot][0] || move == (*killerMoves)[plyFromRoot][1]){
            score += 300;
        }

        score += (*historyTable)[board.sideToMove()][move.from().index()][move.to().index()];


        move.setScore(score);


    }
    auto comparator = [](const Move& a, const Move& b) {
        return a.score() > b.score();
    };

    std::sort(moves.begin(), moves.end(), comparator);
}




