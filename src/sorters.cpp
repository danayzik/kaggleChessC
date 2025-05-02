#include "../include/move_sorters.h"
#include "../include/utilities.h"
#include <algorithm>
#include <limits>

using namespace sorters;
using namespace bot_utils;
using namespace std;



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




