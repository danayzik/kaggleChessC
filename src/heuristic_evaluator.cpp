#include <vector>
#include <map>
#include "../include/evaluator.h"
#include "../include/game_tables.h"
#include "../include/utilities.h"
using namespace bot_utils;
using namespace evaluation;




inline void HeuristicEvaluator::initEval(){
    scores = {0, 0};
    mg = {0, 0};
    eg = {0, 0};
    gamePhase = 0;
}

inline void HeuristicEvaluator::incPestoValues(int colorIndex, int pc, int sqrIndex){
    mg[colorIndex] += mg_table[pc][sqrIndex];
    eg[colorIndex] += eg_table[pc][sqrIndex];
    gamePhase += gamephaseInc[pc];
}

Bitboard HeuristicEvaluator::getAttackSquaresForPiece(PieceType type, Square sq, Bitboard& occ) {
    switch (static_cast<int>(type)) {
        case static_cast<int>(PieceType::BISHOP):
            return attacks::bishop(sq, occ);
        case static_cast<int>(PieceType::ROOK):
            return attacks::rook(sq, occ);
        case static_cast<int>(PieceType::QUEEN):
            return attacks::queen(sq, occ);
        default:
            return {};
    }
}

void HeuristicEvaluator::addPassedPawnBonus(const Board& board, const Square& pawnSquare){
    bool isWhite = workingColor == Color::WHITE;
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, ~workingColor);
    File pawnFile = pawnSquare.file();
    auto pawnFileBits = Bitboard(pawnFile);
    Bitboard tripleFiles = tripleFileMask(pawnFileBits, pawnFile);
    int pawnRank = pawnSquare.rank();
    Bitboard ranksInfrontMask = getRanksInfront(pawnRank, isWhite);
    Bitboard denyingPawns = tripleFiles & enemyPawns & ranksInfrontMask;
    bool passed = denyingPawns == 0;
    scores[workingColorIndex] += passed*passedPawnBonus;

}

void HeuristicEvaluator::addMobilityBonus(const Board& board, const Bitboard& controlledSquares, bool isKing){
    int kingMult = 1 - 2 * isKing;
    Bitboard canReachBits = controlledSquares & (~occ);
    int squareCount = canReachBits.count();
    scores[workingColorIndex] += squareCount*mobilityBonus*kingMult;
}

void HeuristicEvaluator::addOutpostBonus(const Board& board, const Square& knightSquare){
    bool isWhite = workingColor == Color::WHITE;
    // Open File
    Bitboard friendlyPawns = board.pieces(PieceType::PAWN, workingColor);
    File knightFile = knightSquare.file();
    auto knightFileBits = Bitboard(knightFile);
    int halfOpenFile = (knightFileBits & friendlyPawns) == 0;

    // On enemy side
    int knightRank = knightSquare.rank();
    bool isInEnemyTerritory = (workingColorIndex ^ (knightRank >= 4));

    // Protected
    Bitboard friendlyPawnLeftAttacks = isWhite
                                       ? attacks::pawnLeftAttacks<Color::WHITE>(friendlyPawns)
                                       : attacks::pawnLeftAttacks<Color::BLACK>(friendlyPawns);
    Bitboard friendlyPawnRightAttacks = (workingColor == Color::WHITE)
                                        ? attacks::pawnRightAttacks<Color::WHITE>(friendlyPawns)
                                        : attacks::pawnRightAttacks<Color::BLACK>(friendlyPawns);
    Bitboard pawnProtection = friendlyPawnLeftAttacks | friendlyPawnRightAttacks;
    Bitboard knightBoard = 1ULL << knightSquare.index();
    int isDefendedByPawn = (knightBoard & pawnProtection) != 0;

    // Safe From Pawns
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, ~workingColor);
    Bitboard ranksInfrontMask = getRanksInfront(knightRank, isWhite);
    Bitboard sidesMask = sideFilesMask(knightFileBits, knightFile);
    Bitboard denyingPawns = sidesMask & enemyPawns & ranksInfrontMask;
    bool safeFromPawns = denyingPawns == 0;

    scores[workingColorIndex] += (safeFromPawns && isDefendedByPawn && isInEnemyTerritory && halfOpenFile)*outPostBonus;
}

void HeuristicEvaluator::addRookOnOpenFile(const Board& board, const Square& rookSquare){
    Bitboard friendlyPawns = board.pieces(PieceType::PAWN, workingColor);
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, ~workingColor);
    auto rookFile = Bitboard(rookSquare.file());
    int halfOpen = (rookFile & friendlyPawns) == 0;
    int fullyOpen = halfOpen & ((rookFile & enemyPawns) == 0);
    scores[workingColorIndex] += halfOpen * (halfOpenFileBonus + fullyOpen * (fullyOpenFileBonus - halfOpenFileBonus));
}



// Bishop, Rook, Queen
void HeuristicEvaluator::evaluateBlockedPieces(const Board& board){
    for (chess::PieceType type: blockedPieces) {
        int pc = static_cast<int>(type) * 2 + workingColorIndex;
        Bitboard pieceBits = board.pieces(type, workingColor);
        while(pieceBits != 0){
            int sqrIndex = pieceBits.pop();
            incPestoValues(workingColorIndex, pc, sqrIndex);
            Bitboard controlledSquares = getAttackSquaresForPiece(type, sqrIndex, occ);
            addMobilityBonus(board, controlledSquares, false);
            if (type == PieceType::ROOK){
                addRookOnOpenFile(board, Square(sqrIndex));
            }
        }
    }

}

void HeuristicEvaluator::evaluatePawns(const Board& board){

    Bitboard pawnBits = board.pieces(PieceType::PAWN, workingColor);
    int pc = static_cast<int>(PieceType::PAWN) * 2 + workingColorIndex;

    while(pawnBits != 0){
        int sqrIndex = pawnBits.pop();
        auto pawnSquare = Square(sqrIndex);
        addPassedPawnBonus(board, pawnSquare);
        incPestoValues(workingColorIndex, pc, sqrIndex);
    }
}

void HeuristicEvaluator::evaluateKing(const Board& board){
    int pc = static_cast<int>(PieceType::KING) * 2 + workingColorIndex;
    Square kingSquare = board.kingSq(workingColor);
    int sqrIndex = kingSquare.index();
    Bitboard controlledSquares = attacks::king(sqrIndex);
    incPestoValues(workingColorIndex, pc, sqrIndex);
    addMobilityBonus(board, controlledSquares, true);
}

void HeuristicEvaluator::evaluateKnights(const Board& board){
    Bitboard knightBits = board.pieces(PieceType::KNIGHT, workingColor);
    int pc = static_cast<int>(PieceType::KNIGHT) * 2 + workingColorIndex;
    while(knightBits != 0){
        int sqrIndex = knightBits.pop();
        auto knightSquare = Square(sqrIndex);
        Bitboard controlledSquares = attacks::knight(knightSquare);
        incPestoValues(workingColorIndex, pc, sqrIndex);
        addMobilityBonus(board, controlledSquares, false);
        addOutpostBonus(board, knightSquare);
    }
}

int HeuristicEvaluator::getEval(const Board &board) {
    if (gameOver) {
        if(isDraw){
            return 0;
        }
        return winner == Color::BLACK
               ? NEGINF
               : INF;
    }
    int evaluation = 0;
    initEval();
    occ = board.occ();
    Board::CastlingRights rights = board.castlingRights();
    if (rights.has(Color::WHITE)) {
        evaluation += castlingRightsBonus;
    }
    if (rights.has(Color::BLACK)) {
        evaluation -= castlingRightsBonus;
    }
    evaluation += evaluatePieces(board);
    return evaluation;
}

int HeuristicEvaluator::endgameMateEval(const Board& board, int egPhase, int currEval){
    Square whiteKingSquare = board.kingSq(Color::WHITE);
    Square blackKingSquare = board.kingSq(Color::BLACK);
    int minWhiteCornerDistance = 7;
    int minBlackCornerDistance = 7;
    int maxWhiteCenterDistance = 0;
    int maxBlackCenterDistance = 0;
    int whiteKingRank = whiteKingSquare.rank();
    int blackKingRank = blackKingSquare.rank();
    int whiteKingFile = whiteKingSquare.file();
    int blackKingFile = blackKingSquare.file();
    double egWeight = egPhase/2;
    int eval = 0;
    int distanceBetweenKings = abs(whiteKingFile-blackKingFile)+abs(whiteKingRank-blackKingRank);

    for (const auto& corner : cornerSquares) {
        minBlackCornerDistance = std::min(minBlackCornerDistance, Square::distance(blackKingSquare, corner));
        minWhiteCornerDistance = std::min(minWhiteCornerDistance, Square::distance(whiteKingSquare, corner));
    }

    for (const auto& center : centerSquares) {
        maxBlackCenterDistance = std::max(maxBlackCenterDistance, Square::distance(blackKingSquare, center));
        maxWhiteCenterDistance = std::max(maxWhiteCenterDistance, Square::distance(whiteKingSquare, center));

    }
    eval += (4 - maxWhiteCenterDistance)*egWeight;
    eval -= (4 - maxBlackCenterDistance)*egWeight;
    eval += (3 - minBlackCornerDistance)*egWeight;
    eval -= (3 - minWhiteCornerDistance)*egWeight;
    if (currEval>=300){
        eval += (14 - distanceBetweenKings)*egWeight;
    }
    if(currEval <= -300){
        eval -= (14 - distanceBetweenKings)*egWeight;
    }
    return eval;

}

int HeuristicEvaluator::evaluatePieces(const Board& board) {
    Color white = Color::WHITE;
    Color black = ~white;
    for(Color color : {white, black}){
        workingColor = color;
        workingColorIndex = static_cast<int> (workingColor);
        evaluatePawns(board);
        evaluateKnights(board);
        evaluateBlockedPieces(board);
        evaluateKing(board);
    }

    int eval = scores[0] - scores[1];

    int mgScore = mg[0] - mg[1];
    int egScore = eg[0] - eg[1];
    int mgPhase = gamePhase > 24 ? 24 : gamePhase;
    int egPhase = 24 - mgPhase;
    eval += (mgScore * mgPhase + egScore * egPhase) / 24;
    if (egPhase>=16){
        eval += endgameMateEval(board, egPhase, eval);
    }
    return eval;
}

void HeuristicEvaluator::setGameOver(bool isDraw, chess::Color winner) {
    this->isDraw = isDraw;
    this->winner = winner;
    gameOver = true;
}

void HeuristicEvaluator::setGameOngoing() {
    gameOver = false;
}