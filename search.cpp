#include <algorithm>
#include "evaluation.h"
#include "search.h"
#include "transposition_table.cpp"
#include <iostream>
#include <chrono>

TranspositionTable tt(10000, 7000);
Move pv_move = Move();
int currDepth = -1;
std::vector<std::vector<std::vector<int>>> historyTable(2, std::vector<std::vector<int>>(6, std::vector<int>(64, 0)));
std::chrono::time_point<std::chrono::high_resolution_clock> searchStartTime;
int timeLimit = 0;
bool searchStopped = false;

uint64_t getTTKey(Board& board, int depth){
    uint64_t zobrist = board.hash();
    uint64_t shiftedDepth = static_cast<uint64_t>(depth) << 1;
    return zobrist ^ shiftedDepth;
}

void updateHistory(Board& board, const Move& move, int depth, bool isWhite){
    int side = !isWhite;
    int piece = static_cast<int>(board.at(move.from()).type());
    int sqr = move.to().index();
    historyTable[side][piece][sqr] += depth*depth;
}

bool isCheckMove(Board& board, const Move& move) {
    board.makeMove(move);
    bool isCheck = board.inCheck();
    board.unmakeMove(move);
    return isCheck;
}

int fetchHistoryScore(Board& board,Move move){
    int side = static_cast<int>(board.sideToMove());
    int piece = static_cast<int>(board.at(move.from()).type());
    int sqr = move.to().index();
    return historyTable[side][piece][sqr];
}

bool isHashMove(Board& board, const Move& move){
    board.makeMove(move);
    uint64_t key = getTTKey(board, currDepth);
    board.unmakeMove(move);
    return tt.hasKey(key);
}

void addTTEntry(Board& board, int eval, Move bestMove, int depth){
    uint64_t key = getTTKey(board, depth);
    TTEntry entry(eval, bestMove, depth, board.fullMoveNumber());
    tt.store(key, entry);
}

std::pair<GameResultReason, GameResult> isGameOver(Board& board, Movelist& legalMovelist)  {
    if (board.isHalfMoveDraw()) return board.getHalfMoveDrawType();
    if (board.isInsufficientMaterial()) return {GameResultReason::INSUFFICIENT_MATERIAL, GameResult::DRAW};
    if (board.isRepetition()) return {GameResultReason::THREEFOLD_REPETITION, GameResult::DRAW};
    if (legalMovelist.empty()) {
        if (board.inCheck()) return {GameResultReason::CHECKMATE, GameResult::LOSE};
        return {GameResultReason::STALEMATE, GameResult::DRAW};
    }

    return {GameResultReason::NONE, GameResult::NONE};
}

int mvvLvaScore(const Board& board, const Move& move){
    Square from = move.from();
    Square to = move.to();
    int attacker = static_cast<int>(board.at(from).type());
    int victim = static_cast<int>(board.at(to).type());
    return victim * 10 - attacker;
}

void sortCaptureList(Board& board, Movelist& moves){
    auto comparator = [&board](const Move &a, const Move &b) {
        return mvvLvaScore(board, a) > mvvLvaScore(board, b);
    };
    std::sort(moves.begin(), moves.end(), comparator);
}

void sortMovelist(Board& board, Movelist& moves) {
    auto comparator = [&board](const Move &a, const Move &b) {
        bool leftIsPV = a == pv_move;
        bool rightIsPV = b == pv_move;
        if (leftIsPV && !rightIsPV) return true;
        if (!leftIsPV && rightIsPV) return false;
        bool leftIsHash = isHashMove(board, a);
        bool rightIsHash = isHashMove(board, b);
        if (leftIsHash && !rightIsHash) return true;
        if (!leftIsHash && rightIsHash) return false;
        bool leftIsCheckMove = isCheckMove(board, a);
        bool rightIsCheckMove = isCheckMove(board, b);
        if (leftIsCheckMove && !rightIsCheckMove) return true;
        if (!leftIsCheckMove && rightIsCheckMove) return false;
        bool leftIsCapture = board.isCapture(a);
        bool rightIsCapture = board.isCapture(b);
        if(leftIsCapture && rightIsCapture){
            return mvvLvaScore(board, a) > mvvLvaScore(board, b);
        }
        if(leftIsCapture && !rightIsCapture){
            return true;
        }
        if(!leftIsCapture && rightIsCapture){
            return false;
        }
        return fetchHistoryScore(board, a) > fetchHistoryScore(board, b);
    };
    std::sort(moves.begin(), moves.end(), comparator);
}

std::pair<int , Move> iterativeSearch(Board& board, bool is_maximizing, int msTimeLimit){
    searchStopped = false;
    searchStartTime = std::chrono::high_resolution_clock::now();
    timeLimit = msTimeLimit;
    int eval = 0;
    Move bestMove = Move();
    std::pair<int, Move> miniMaxRes;
    int alpha = NEGINF;
    int beta = INF;
    int depth = 1;
    std::string t;

    while(true) {
        auto start = std::chrono::high_resolution_clock::now();
        miniMaxRes = minimax(board, depth, alpha, beta, is_maximizing, false);
//        auto end = std::chrono::high_resolution_clock::now();
//        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//        std::cout << "Elapsed time for depth: " << depth<<" " << duration.count() << " microseconds" << std::endl;
//        auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
        if ((!searchStopped)) {
            eval = miniMaxRes.first;
            bestMove = miniMaxRes.second;
            t = uci::moveToUci(bestMove);
//            std::cout << "Best move for depth: "<< depth << " " << t<<std::endl;
            pv_move = bestMove;
        }
        else{break;}
//        if (elapsed >= std::chrono::milliseconds(msTimeLimit))

//        if(eval < alpha || eval > beta)
//        {
//            alpha = NEGINF;
//            beta = INF;
//            continue;
//        }
//        alpha = eval - 200;
//        beta = eval + 200;
        depth++;
    }
//    std::cout << depth-1 << std::endl<<std::flush;
    return {eval, bestMove};
}


std::pair<int , Move> minimax(Board& board, int depth, int alpha, int beta, bool is_maximizing, bool inSearch) {
    auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
    if (elapsed >= std::chrono::milliseconds(timeLimit)) {
        searchStopped = true;
        return {0, Move()};
    }
    currDepth = depth;
    int moveIndex = 0;
    Movelist moves;
    movegen::legalmoves(moves, board);
    auto [resultReason, gameResult] = isGameOver(board, moves);
    if(resultReason != GameResultReason::NONE || depth == 0)
        return {evaluate_board(board, gameResult, resultReason), Move()};
//    if (depth == 0) {
//        return quiesce(board, alpha, beta, is_maximizing);
//    }
    uint64_t key = getTTKey(board, depth);
    if (tt.hasKey(key)) {
        return tt.fetch(key);
    }
    Move best_move = Move();
    sortMovelist(board, moves);
    if (is_maximizing) {
        int max_eval = NEGINF - 1;
        for (auto move: moves) {
            moveIndex++;
            bool isCapture = board.isCapture(move);
            board.makeMove(move);
            int lateMoveReduction = moveIndex >= 4 && depth > 3;
            bool checkMove = board.inCheck();
            int extension = checkMove || isCapture - lateMoveReduction;
            if(!inSearch)
                extension = 0;
            auto [eval, _] = minimax(board, depth - 1 + extension, alpha, beta, false, true);
            board.unmakeMove(move);
            if (eval > max_eval) {
                best_move = move;
            }
            max_eval = std::max(eval, max_eval);
            alpha = std::max(alpha, eval);
            if (eval >= beta && inSearch) {
                if (!board.isCapture(move))
                    updateHistory(board, move, depth, is_maximizing);
                break;
            }
        }
        addTTEntry(board, max_eval, best_move, depth);
        return {max_eval, best_move};
    } else {
        int min_eval = INF + 1;
        for (auto move: moves) {
            moveIndex++;
            bool isCapture = board.isCapture(move);
            board.makeMove(move);
            bool checkMove = board.inCheck();
            int lateMoveReduction = moveIndex >= 4 && depth > 3;
            int extension = checkMove || isCapture - lateMoveReduction;
            if(!inSearch)
                extension = 0;
            auto [eval, _] = minimax(board, depth - 1 + extension, alpha, beta, true, true);
            board.unmakeMove(move);
            if (elapsed >= std::chrono::milliseconds(timeLimit)) {
                searchStopped = true;
                return {0, move};
            }
            if (eval < min_eval) {
                best_move = move;
            }
            min_eval = std::min(min_eval, eval);
            beta = std::min(beta, eval);
            if (eval <= alpha && inSearch) {
                if (!board.isCapture(move))
                    updateHistory(board, move, depth, is_maximizing);
                break;
            }
        }
        addTTEntry(board, min_eval, best_move, depth);
        return {min_eval, best_move};
    }
}

//std::pair<int , Move> quiesce(Board& board, int alpha, int beta, bool is_maximizing){
//    auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
//    if (elapsed >= std::chrono::milliseconds(timeLimit)){
//        searchStopped = true;
//        return {0, Move()};
//    }
//    Movelist moves;
//    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board);
//    auto [resultReason, gameResult] = board.isGameOver();
//    if (resultReason != GameResultReason::NONE || moves.empty()) {
//        return {evaluate_board(board, gameResult, resultReason), Move()};
//    }
//    Move best_move = Move();
//    sortCaptureList(board, moves);
//    if (is_maximizing) {
//        int max_eval = NEGINF-1;
//        for (auto move : moves) {
//            board.makeMove(move);
//            auto [eval, _] = quiesce(board, alpha, beta, false);
//            board.unmakeMove(move);
//            if (eval > max_eval) {
//                best_move = move;
//            }
//            max_eval = std::max(eval, max_eval);
//            alpha = std::max(alpha, eval);
//            if (eval >= beta) {
//                break;
//            }
//        }
//        return {max_eval, best_move};
//    }
//    else {
//        int min_eval = INF + 1;
//        for (auto move: moves) {
//            board.makeMove(move);
//            auto [eval, _] = quiesce(board, alpha, beta, true);
//            board.unmakeMove(move);
//            if (elapsed >= std::chrono::milliseconds(timeLimit)) {
//                searchStopped = true;
//                return {0, move};
//            }
//            if (eval < min_eval)
//                best_move = move;
//            min_eval = std::min(min_eval, eval);
//            beta = std::min(beta, eval);
//            if (eval <= alpha) {
//                break;
//            }
//
//        }
//        return {min_eval, best_move};
//    }
//}
std::pair<int , Move> quiesce(Board& board, int alpha, int beta, bool isWhiteToMove){

    auto elapsed = std::chrono::high_resolution_clock::now() - searchStartTime;
    if (elapsed >= std::chrono::milliseconds(timeLimit)){
        searchStopped = true;
        return {0, Move()};
    }
    Movelist moves;
    movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board);
    auto [resultReason, gameResult] = board.isGameOver();
    if (resultReason != GameResultReason::NONE || moves.empty()) {
        return {evaluate_board(board, gameResult, resultReason), Move()};
    }
    int staticEval = evaluate_board(board, gameResult, resultReason);
    int stand_pat = isWhiteToMove ? staticEval : -staticEval;
    int bestEval = stand_pat;
    if (stand_pat >= beta)
        return {stand_pat, Move()};
    if (alpha < stand_pat)
        alpha = stand_pat;
    Move bestMove = Move();
    sortCaptureList(board, moves);
    for (auto move : moves) {
        board.makeMove(move);
        auto [eval, _] = quiesce(board, -alpha, -beta, !isWhiteToMove);
        eval = -eval;
        board.unmakeMove(move);
        if (eval >= beta)
            return {eval, bestMove};
        if (eval > bestEval){
            bestEval = eval;
            bestMove = move;
        }
        if (eval > alpha)
            alpha = eval;
    }
    return {bestEval, bestMove};

}




