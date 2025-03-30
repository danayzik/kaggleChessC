#include "../include/chess.hpp"
#include "../include/game_tables.h"
#include "../include/engine.h"

using namespace std;
using namespace chess;
using namespace engines;



void run(Engine& engine){
    while (true){
        std::string move_uci;
        std::getline(std::cin, move_uci);
        if (move_uci == "end") {
            break;
        }
        if (move_uci != "start") {
            Move enemyMove = uci::uciToMove(engine.getBoard() ,move_uci);
            engine.makeMove(enemyMove);
        }
        Move move = engine.getMove();
        string moveUCI = uci::moveToUci(move);
        engine.makeMove(move);
        std::cout << moveUCI << std::endl << std::flush;
    }
}



Color getColor(){
    std::string input;
    std::getline(std::cin, input);
    char color = input[0];
    return color == 'w'? Color::WHITE : Color::BLACK;

}


int main() {
    string fen;
    initTables();
    Color color = getColor();
    bool isWhite = color == Color::WHITE;
    getline(std::cin, fen);
    Board myBoard = Board(fen);
    Evaluator* evaluator = new HeuristicEvaluator();
    Searcher* searcher = new BasicSearcher(evaluator, isWhite);
    Engine myEngine = Engine(std::move(myBoard), color, evaluator, searcher);
    run(myEngine);
    return 0;
}

