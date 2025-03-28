#ifndef SEARCH_H
#define SEARCH_H

#include "chess.hpp"
using namespace chess;

bool isCheckMove(Board& board, const Move& move);
void sortMovelist(Board& board, Movelist& moves);
std::pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool is_maximizing, bool inSearch);
std::pair<int , Move> iterativeSearch(Board& board, bool is_maximizing, int msTimeLimit);
std::pair<int , Move> quiesce(Board& board, int alpha, int beta, bool isWhiteToMove);

#endif // SEARCH_H
