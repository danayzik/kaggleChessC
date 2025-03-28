#ifndef EVALUATION_H
#define EVALUATION_H

#include "chess.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
using namespace chess;

constexpr int INF = 32767;
constexpr int NEGINF = -32768;
constexpr int castle_rights_bonus = 10;

extern std::map<PieceType, int> ATTACK_VALUES;

extern std::map<PieceType, int> DEFEND_VALUES;

extern std::vector<PieceType> PIECETYPES;

extern std::array<int, 2> scores;
extern std::array<int, 2> mg;
extern std::array<int, 2> eg;
extern int gamePhase;

void initEval();

void incPestoValues(int colorIndex, int pc, int sqrIndex);

void addAttackDefendValues(const Board& board, Bitboard controlledSquares, Color color);

void evaluateBlockedPieces(const Board& board, Color color);

void evaluatePawns(const Board& board, Color color);

void evaluateKing(const Board& board, Color color);

void evaluateKnights(const Board& board, Color color);

int evaluatePieces(const chess::Board& board);

int evaluate_board(const Board& board, GameResult result, GameResultReason reason);

int endgameMateEval(const Board& board, int egPhase, int currEval);
#endif // EVALUATION_H
