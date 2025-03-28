#include "../include/chess.hpp"
#include "../include/game_tables.h"
#include "../include/search.h"

Board myBoard;
bool maximizeSearch;
int getTimeLimit(){
    return myBoard.fullMoveNumber() >= 55? 100: 250;
}

void run(){
    while (true){
        std::string move_uci;
        std::getline(std::cin, move_uci);
        if (move_uci == "end") {
            break;
        }
        if (move_uci != "start") {
            Move enemyMove = uci::uciToMove(myBoard ,move_uci);
            myBoard.makeMove(enemyMove);
        }
        auto [eval, move] = iterativeSearch(myBoard, maximizeSearch, getTimeLimit());
        std::string moveUCI = uci::moveToUci(move);
        myBoard.makeMove(move);
        std::cout << moveUCI << std::endl << std::flush;
    }
}

void setupBoard(){
    std::string fen;
    std::getline(std::cin, fen);
    myBoard = Board(fen);
}

void setColor(){
    std::string input;
    std::getline(std::cin, input);
    char color = input[0];
    if (color == 'w')maximizeSearch = true;
    if (color == 'b')maximizeSearch = false;
}


int main() {
    initTables();
    setColor();
    setupBoard();
    run();
    return 0;
}

