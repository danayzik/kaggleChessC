
#include "../include/transposition_table.h"
#include "../include/searchers.h"
#include "../include/utilities.h"
using namespace bot_utils;
using namespace searchers;
using namespace chess;



IterativePvHistoryKillerSearcher::IterativePvHistoryKillerSearcher(Evaluator* evaluator, bool isWhite) : Searcher(evaluator, isWhite, std::make_unique<PvHistoryKillerMoveSorter>()) {
    sorter->setHistoryTable(&historyTable); sorter->setKillerMoves(&killerMoves);}

Move IterativePvHistoryKillerSearcher::getMove(chess::Board &board, int msTimeLimit) {
    searchStartTime = std::chrono::high_resolution_clock::now();
    searchTimeLimit = msTimeLimit;
    auto [eval, move] = iterativeDeepening(board);
    return move;
}

pair<int, Move> IterativePvHistoryKillerSearcher::iterativeDeepening(chess::Board &board) {
    int newEval = 0;
    int currEval = 0;
    Move bestMove = Move();
    int alpha = NEGINF;
    int beta = INF;
    int depth = 1;
    stopSearch = false;
    while(depth < 40) {
        validSearchResult = false;
        std::pair<int, Move> miniMaxRes = minimax(board, depth, alpha, beta, isWhite, true);
        newEval = miniMaxRes.first;
        if(stopSearch){
            bool betterEval = isWhite ? newEval > currEval : newEval < currEval;
            if(validSearchResult && betterEval){
                currEval = newEval;
                bestMove = miniMaxRes.second;
            }
            break;
        }
        currEval = newEval;
        bestMove = miniMaxRes.second;
        sorter->setPv(bestMove);
        depth++;
    }
    decayHistory();
    return {currEval, bestMove};
}

pair<int, Move> IterativePvHistoryKillerSearcher::minimax(Board &board, int depth, int alpha, int beta, bool isMaximizing, bool inRoot) {

    auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
    if (elapsed >= std::chrono::milliseconds(searchTimeLimit)) {
        stopSearch = true;
        return {0, Move()};
    }
    sorter->setDepth(depth);
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
        for (const auto& move: moves) {
            bool isCapture = board.isCapture(move);
            board.makeMove(move);
            auto [eval, _] = minimax(board, depth - 1, alpha, beta, false, false);
            board.unmakeMove(move);
            if(!stopSearch){
                if(inRoot)
                    validSearchResult = true;
            }
            else{
                return {max_eval, best_move};
            }
            if (eval > max_eval) {
                best_move = move;
            }
            max_eval = max(eval, max_eval);
            alpha = max(alpha, eval);
            if (eval >= beta ) {
                if (!isCapture && !board.inCheck()) {
                    killerMoves[depth][1] = killerMoves[depth][0];
                    killerMoves[depth][0] = move;
                }
                historyTable[move.from().index()][move.to().index()] += depth * depth;
                break;
            }
        }
        return {max_eval, best_move};
    } else {
        int min_eval = INF + 1;
        for (const auto& move: moves) {
            bool isCapture = board.isCapture(move);
            board.makeMove(move);
            auto [eval, _] = minimax(board, depth - 1 , alpha, beta, true, false);
            board.unmakeMove(move);
            if(!stopSearch){
                if(inRoot)
                    validSearchResult = true;
            }
            else{
                return {min_eval, best_move};
            }
            if (eval < min_eval) {
                best_move = move;
            }
            min_eval = min(min_eval, eval);
            beta = min(beta, eval);
            if (eval <= alpha) {
                if (!isCapture && !board.inCheck()) {
                    killerMoves[depth][1] = killerMoves[depth][0];
                    killerMoves[depth][0] = move;
                }
                historyTable[move.from().index()][move.to().index()] += depth * depth;
                break;
            }
        }
        return {min_eval, best_move};
    }

}







