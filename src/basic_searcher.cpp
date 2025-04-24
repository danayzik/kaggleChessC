
#include "../include/transposition_table.h"
#include "../include/searchers.h"
#include "../include/utilities.h"
using namespace bot_utils;
using namespace searchers;
using namespace chess;





BasicSearcher::BasicSearcher(Evaluator* evaluator, bool isWhite)
        : Searcher(evaluator, isWhite, std::make_unique<BasicMoveSorter>()) {}

Move BasicSearcher::getMove(Board &board, int msTimeLimit) {
    auto [eval, move] = minimax(board, maxDepth, NEGINF, INF, isWhite);
    return move;
}


pair<int, Move> BasicSearcher::minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing) {
    Movelist moves;
    movegen::legalmoves(moves, board);
    auto [resultReason, gameResult] = isGameOver(board, moves);
    bool gameOver = resultReason != GameResultReason::NONE;
    if(gameOver){
        Color winner = board.sideToMove() == Color::WHITE ? Color::BLACK : Color::WHITE;
        evaluator->setGameOver(gameResult == GameResult::DRAW, winner);
        return {evaluator->getEval(board), Move()};
    }

    if(depth == 0) {
        evaluator->setGameOngoing();
        return {evaluator->getEval(board), Move()};
    }
    Move best_move = Move();
    sorter->sortMovelist(board, moves);
    if (isMaximizing) {
        int max_eval = NEGINF - 1;
        for (auto move: moves) {
            board.makeMove(move);
            auto [eval, _] = minimax(board, depth - 1, alpha, beta, false);
            board.unmakeMove(move);
            if (eval > max_eval) {
                best_move = move;
            }
            max_eval = max(eval, max_eval);
            alpha = max(alpha, eval);
            if (eval >= beta ) {
                break;
            }
        }
        return {max_eval, best_move};
    } else {
        int min_eval = INF + 1;
        for (auto move: moves) {
            board.makeMove(move);
            auto [eval, _] = minimax(board, depth - 1 , alpha, beta, true);
            board.unmakeMove(move);
            if (eval < min_eval) {
                best_move = move;
            }
            min_eval = min(min_eval, eval);
            beta = min(beta, eval);
            if (eval <= alpha) {
                break;
            }
        }
        return {min_eval, best_move};
    }
}






