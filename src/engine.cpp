#include "../include/engine.h"

using namespace std;
using namespace chess;
using namespace engines;


Engine::Engine(Board&& board, Color myColor, Evaluator* evaluator, Searcher* searcher) : board(std::move(board)), myColor(myColor), evaluator(evaluator),
                                                                                      searcher(searcher) {isWhite = myColor == Color::WHITE;}

[[maybe_unused]] int Engine::getEval() {return evaluator->getEval(board);}

Move Engine::getMove() {return searcher->getMove(board, 150);}//Add time manager


void Engine::makeMove(const chess::Move &move) {board.makeMove(move);}

const Board &Engine::getBoard() {return board;}

