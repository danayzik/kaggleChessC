#include "../include/engine.h"
#include "../include/utilities.h"
using namespace std;
using namespace chess;
using namespace engines;
using namespace bot_utils;


Engine::Engine(Board&& board, Color myColor, Evaluator* evaluator, Searcher* searcher) : board(std::move(board)), myColor(myColor), evaluator(evaluator),
                                                                                      searcher(searcher) {isWhite = myColor == Color::WHITE;}

[[maybe_unused]] int Engine::getEval() {return evaluator->getEval(board);}

Move Engine::getMove() {
    auto [reason, result] = board.isGameOver();
    if(reason != GameResultReason::NONE)
        exit(0);
    return searcher->getMove(board, 200);
}//Add time manager


void Engine::makeMove(const chess::Move &move) {board.makeMove(move);}

const Board &Engine::getBoard() {return board;}

