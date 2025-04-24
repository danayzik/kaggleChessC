
#include "../include/transposition_table.h"
#include "../include/searchers.h"
#include "../include/utilities.h"
using namespace bot_utils;
using namespace searchers;
using namespace chess;





BasicIterativeSearcher::BasicIterativeSearcher(Evaluator* evaluator, bool isWhite)
        : Searcher(evaluator, isWhite, std::make_unique<BasicMoveSorter>()) {}

Move BasicIterativeSearcher::getMove(Board &board, int msTimeLimit) {
    searchStartTime = std::chrono::high_resolution_clock::now();
    searchTimeLimit = msTimeLimit;
    auto [eval, move] = iterativeDeepening(board);
    return move;
}



pair<int, Move> BasicIterativeSearcher::iterativeDeepening(chess::Board &board) {
    int newEval = 0;
    int currEval = 0;
    Move bestMove = Move();
    int alpha = NEGINF;
    int beta = INF;
    int depth = 1;
    stopSearch = false;

    while(true) {
        validSearchResult = false;
        std::pair<int, Move> miniMaxRes = minimax(board, depth, alpha, beta, isWhite);
        newEval = miniMaxRes.first;
        if(stopSearch){
            break;
        }
        currEval = newEval;
        bestMove = miniMaxRes.second;

        depth++;
    }
    return {currEval, bestMove};
}


pair<int, Move> BasicIterativeSearcher::minimax(Board& board, int depth, int alpha, int beta, bool isMaximizing) {
    auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
    if (elapsed >= std::chrono::milliseconds(searchTimeLimit)) {
        stopSearch = true;
        return {0, Move()};
    }
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






