
#include "../include/transposition_table.h"
#include "../include/searchers.h"
#include "../include/utilities.h"
using namespace bot_utils;
using namespace searchers;
using namespace chess;

int hitCount = 0;
int nodeCount = 0;
int semiHitCount = 0;
IterativePvHistoryKillerTTSearcher::IterativePvHistoryKillerTTSearcher(Evaluator* evaluator, bool isWhite) : Searcher(evaluator, isWhite, std::make_unique<PvHistoryKillerTTMoveSorter>()) {
    sorter->setHistoryTable(&historyTable); sorter->setKillerMoves(&killerMoves); sorter->setTT(&transpositionTable);}

Move IterativePvHistoryKillerTTSearcher::getMove(chess::Board &board, int msTimeLimit) {
    searchStartTime = std::chrono::high_resolution_clock::now();
    searchTimeLimit = msTimeLimit;
    auto [eval, move] = iterativeDeepening(board);
    return move;
}

pair<int, Move> IterativePvHistoryKillerTTSearcher::iterativeDeepening(chess::Board &board) {
    int newEval = 0;
    int currEval = 0;
    Move bestMove = Move();
    int alpha = NEGINF;
    int beta = INF;
    int depth = 1;
    stopSearch = false;
    while(depth < MAX_DEPTH) {
        validSearchResult = false;
        std::pair<int, Move> miniMaxRes = minimax(board, depth, 0, alpha, beta, isWhite, true);
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
    std::cout << "Depth: " << depth << std::endl;
    std::cout<< "Hit count: " << hitCount << std::endl << "Node count: " << nodeCount << std::endl;
    std::cout<< "SemiHit count: " << semiHitCount << std::endl;
    hitCount = 0;
    nodeCount = 0;
    semiHitCount = 0;
    return {currEval, bestMove};
}

pair<int, Move> IterativePvHistoryKillerTTSearcher::minimax(Board &board, int depth, int plyFromRoot, int alpha, int beta, bool isMaximizing, bool inRoot) {
    nodeCount++;
    auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
    if (elapsed >= std::chrono::milliseconds(searchTimeLimit)) {
        stopSearch = true;
        return {0, Move()};
    }
    sorter->setDepths(depth, plyFromRoot);
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
    TTEntry entry{};
    bool hasKey = transpositionTable.probe(board.hash(), depth, entry);
    Move best_move = Move();
    if(hasKey){
        switch (entry.entryType) {
            case EntryType::EXACT:
                hitCount++;
                semiHitCount++;
                return {entry.eval, entry.bestMove};
            case EntryType::LOWERBOUND:
                if(isMaximizing){
                    if(entry.eval >= beta){
                        hitCount++;
                        return {entry.eval, entry.bestMove};
                    }
                    semiHitCount++;
                    alpha = max(alpha, entry.eval);
                    best_move = entry.bestMove;
                    break;

                }
                break;
            case EntryType::UPPERBOUND:
                if(!isMaximizing){
                    if(entry.eval <= alpha){
                        hitCount++;
                        return {entry.eval, entry.bestMove};
                    }
                    semiHitCount++;
                    best_move = entry.bestMove;
                    beta = min(beta, entry.eval);
                }
        }
    }


    sorter->sortMovelist(board, moves);
    if (isMaximizing) {
        int max_eval = NEGINF - 1;
        for (const auto& move: moves) {
            bool isCapture = board.isCapture(move);
            board.makeMove(move);
            bool isCheck = board.inCheck();
            auto [eval, _] = minimax(board, depth - 1, plyFromRoot+1, alpha, beta, false, false);
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
                max_eval = eval;
            }
            alpha = max(alpha, eval);
            if (eval >= beta ) {
                transpositionTable.store(board.hash(), max_eval, best_move, depth, EntryType::LOWERBOUND, board.fullMoveNumber());
                if (!isCapture && !isCheck) {
                    killerMoves[plyFromRoot][1] = killerMoves[plyFromRoot][0];
                    killerMoves[plyFromRoot][0] = move;
                }
                historyTable[board.sideToMove()][move.from().index()][move.to().index()] += depth * depth;
                return {max_eval, best_move};
            }
        }
        transpositionTable.store(board.hash(), max_eval, best_move, depth, EntryType::EXACT, board.fullMoveNumber());
        return {max_eval, best_move};
    } else {
        int min_eval = INF + 1;
        for (const auto& move: moves) {
            bool isCapture = board.isCapture(move);
            board.makeMove(move);
            bool isCheck = board.inCheck();
            auto [eval, _] = minimax(board, depth - 1, plyFromRoot+1 , alpha, beta, true, false);
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
                min_eval = eval;
            }
            beta = min(beta, eval);
            if (eval <= alpha) {
                transpositionTable.store(board.hash(), min_eval, best_move, depth, EntryType::UPPERBOUND, board.fullMoveNumber());
                if (!isCapture && !isCheck) {
                    killerMoves[plyFromRoot][1] = killerMoves[plyFromRoot][0];
                    killerMoves[plyFromRoot][0] = move;
                }
                historyTable[board.sideToMove()][move.from().index()][move.to().index()] += depth * depth;
                return {min_eval, best_move};
            }
        }
        transpositionTable.store(board.hash(), min_eval, best_move, depth, EntryType::EXACT, board.fullMoveNumber());
        return {min_eval, best_move};
    }

}







