#include <vector>
#include <map>
#include "evaluation.h"
#include "game_tables.h"


std::array<int, 2> scores;
std::array<int, 2> mg;
std::array<int, 2> eg;
int gamePhase;

std::map<PieceType, int> ATTACK_VALUES = {
        {PieceType::PAWN, 3},
        {PieceType::KNIGHT, 6},
        {PieceType::BISHOP, 6},
        {PieceType::ROOK, 6},
        {PieceType::QUEEN, 8},
        {PieceType::KING, 8}
};

std::map<PieceType, int> DEFEND_VALUES = {
        {PieceType::PAWN, 10},
        {PieceType::KNIGHT, 5},
        {PieceType::BISHOP, 5},
        {PieceType::ROOK, 3},
        {PieceType::QUEEN, 0},
        {PieceType::KING, 0}
};

std::vector<PieceType> PIECETYPES = {
        PieceType::PAWN,
        PieceType::KNIGHT,
        PieceType::BISHOP,
        PieceType::ROOK,
        PieceType::QUEEN,
        PieceType::KING
};

int mobilityBonus = 4;
int halfOpenFileBonus = 25;
int fullyOpenFileBonus = 60;
int outPostBonus = 80;
int passedPawnBonus = 40;

Bitboard tripleFileMask(Bitboard fileBits, File f){
    Bitboard leftFile = f >> std::min(1, (int)f);
    Bitboard rightFile = f >> std::min(1, (int)f-7);
    return  (leftFile | rightFile | fileBits);
}

Bitboard sideFilesMask(Bitboard fileBits, File f){
    Bitboard tripleMask = tripleFileMask(fileBits, f);
    return tripleMask & (~fileBits);
}

void initEval(){
    scores = {0, 0};
    mg = {0, 0};
    eg = {0, 0};
    gamePhase = 0;
}

void incPestoValues(int colorIndex, int pc, int sqrIndex){
    mg[colorIndex] += mg_table[pc][sqrIndex];
    eg[colorIndex] += eg_table[pc][sqrIndex];
    gamePhase += gamephaseInc[pc];
}

Bitboard getRanksInfront(int pieceRank, bool isWhite){
    uint64_t allOnes = ~0ULL;
    Bitboard full = Bitboard(allOnes);

    Bitboard ranksInfrontMask = isWhite? full << 8*(pieceRank + 1): full >> 8*(8-pieceRank);
    return ranksInfrontMask;
}

void addPassedPawnBonus(const Board& board, Square pawnSquare, Color color){
    int colorIndex = static_cast<int>(color);
    bool isWhite = color == Color::WHITE;
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, ~color);
    File pawnFile = pawnSquare.file();
    Bitboard pawnFileBits = Bitboard(pawnFile);
    Bitboard tripleFiles = tripleFileMask(pawnFileBits, pawnFile);
    int pawnRank = pawnSquare.rank();
    Bitboard ranksInfrontMask = getRanksInfront(pawnRank, isWhite);
    Bitboard denyingPawns = tripleFiles & enemyPawns & ranksInfrontMask;
    bool passed = denyingPawns == 0;
    scores[colorIndex] += passed*passedPawnBonus;


}

void addMobilityBonus(const Board& board, Bitboard controlledSquares, Color color, bool isKing){
    int colorIndex = static_cast<int>(color);
    int kingMult = 1 - 2 * isKing;
    Bitboard occ = board.occ();
    Bitboard canReachBits = controlledSquares & (~occ);
    int squareCount = canReachBits.count();
    scores[colorIndex] += squareCount*mobilityBonus*kingMult;
}

void addOutpostBonus(const Board& board, Square knightSquare, Color color){
    int colorIndex = static_cast<int>(color);
    bool isWhite = color == Color::WHITE;
    // Open File
    Bitboard friendlyPawns = board.pieces(PieceType::PAWN, color);
    File knightFile = knightSquare.file();
    Bitboard knightFileBits = Bitboard(knightFile);
    int halfOpenFile = (knightFileBits & friendlyPawns) == 0;

    // On enemy side
    int knightRank = knightSquare.rank();
    bool isInEnemyTerritory = (colorIndex ^ (knightRank >= 4));

    // Protected
    Bitboard friendlyPawnLeftAttacks = isWhite
                                       ? attacks::pawnLeftAttacks<Color::WHITE>(friendlyPawns)
                                       : attacks::pawnLeftAttacks<Color::BLACK>(friendlyPawns);
    Bitboard friendlyPawnRightAttacks = (color == Color::WHITE)
                                        ? attacks::pawnRightAttacks<Color::WHITE>(friendlyPawns)
                                        : attacks::pawnRightAttacks<Color::BLACK>(friendlyPawns);
    Bitboard pawnProtection = friendlyPawnLeftAttacks | friendlyPawnRightAttacks;
    Bitboard knightBoard = 1ULL << knightSquare.index();
    int isDefendedByPawn = (knightBoard & pawnProtection) != 0;

    // Safe From Pawns
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, ~color);
    Bitboard ranksInfrontMask = getRanksInfront(knightRank, isWhite);
    Bitboard sidesMask = sideFilesMask(knightFileBits, knightFile);
    Bitboard denyingPawns = sidesMask & enemyPawns & ranksInfrontMask;
    bool safeFromPawns = denyingPawns == 0;

    scores[colorIndex] += (safeFromPawns && isDefendedByPawn && isInEnemyTerritory && halfOpenFile)*outPostBonus;
}

void addRookOnOpenFile(const Board& board, Square rookSquare, Color color){
    int colorIndex = static_cast<int>(color);
    Bitboard friendlyPawns = board.pieces(PieceType::PAWN, color);
    Bitboard enemyPawns = board.pieces(PieceType::PAWN, ~color);
    Bitboard rookFile = Bitboard(rookSquare.file());
    int halfOpen = (rookFile & friendlyPawns) == 0;
    int fullyOpen = halfOpen & ((rookFile & enemyPawns) == 0);
    scores[colorIndex] += halfOpen * (halfOpenFileBonus + fullyOpen * (fullyOpenFileBonus - halfOpenFileBonus));
}

void addAttackDefendValues(const Board& board, Bitboard controlledSquares, Color color){
    int colorIndex = static_cast<int>(color);
    for(PieceType type : PIECETYPES){
        Bitboard enemyPieceBitboard = board.pieces(type, ~color);
        Bitboard friendlyPieceBitboard = board.pieces(type, color);
        Bitboard attackedPieces = enemyPieceBitboard & controlledSquares;
        Bitboard defendedPieces = friendlyPieceBitboard & controlledSquares;
        scores[colorIndex] += attackedPieces.count() * ATTACK_VALUES[type];
        scores[colorIndex] += defendedPieces.count() * DEFEND_VALUES[type];
    }
}

// Bishop, Rook, Queen
void evaluateBlockedPieces(const Board& board, Color color){
    std::vector<PieceType> blockedPieces = {PieceType::BISHOP, PieceType::ROOK, PieceType::QUEEN};
    std::map<PieceType, std::function<Bitboard(Square, Bitboard)>> pieceTypeToAttackFunction = {
            {PieceType::BISHOP, attacks::bishop},
            {PieceType::ROOK, attacks::rook},
            {PieceType::QUEEN, attacks::queen}
    };
    Bitboard occ = board.occ();
    int colorIndex = static_cast<int>(color);
    for (chess::PieceType type: blockedPieces) {
        int pc = static_cast<int>(type) * 2 + colorIndex;
        Bitboard pieceBits = board.pieces(type, color);
        while(pieceBits != 0){
            int sqrIndex = pieceBits.pop();
            incPestoValues(colorIndex, pc, sqrIndex);
            Bitboard controlledSquares = pieceTypeToAttackFunction[type](sqrIndex, occ);
            addAttackDefendValues(board, controlledSquares, color);
            addMobilityBonus(board, controlledSquares, color, false);
            if (type == PieceType::ROOK){
                addRookOnOpenFile(board, Square(sqrIndex), color);
            }
        }
    }

}

void evaluatePawns(const Board& board, Color color){
    Bitboard leftAttacks;
    Bitboard rightAttacks;
    Bitboard pawnBits = board.pieces(PieceType::PAWN, color);
    int colorIndex = static_cast<int>(color);
    int pc = static_cast<int>(PieceType::PAWN) * 2 + colorIndex;
    if (color == Color::WHITE) {
        leftAttacks = attacks::pawnLeftAttacks<Color::WHITE>(pawnBits);
        rightAttacks = attacks::pawnRightAttacks<Color::WHITE>(pawnBits);
    }
    else{
        leftAttacks = attacks::pawnLeftAttacks<Color::BLACK>(pawnBits);
        rightAttacks = attacks::pawnRightAttacks<Color::BLACK>(pawnBits);
    }
    addAttackDefendValues(board, leftAttacks, color);
    addAttackDefendValues(board, rightAttacks, color);

    while(pawnBits != 0){
        int sqrIndex = pawnBits.pop();
        Square pawnSquare = Square(sqrIndex);
        addPassedPawnBonus(board, pawnSquare, color);
        incPestoValues(colorIndex, pc, sqrIndex);
    }
}

void evaluateKing(const Board& board, Color color){
    int colorIndex = static_cast<int>(color);
    int pc = static_cast<int>(PieceType::KING) * 2 + colorIndex;
    Square kingSquare = board.kingSq(color);
    int sqrIndex = kingSquare.index();
    Bitboard controlledSquares = attacks::king(sqrIndex);
    incPestoValues(colorIndex, pc, sqrIndex);
    addAttackDefendValues(board, controlledSquares, color);
    addMobilityBonus(board, controlledSquares, color, true);
}

void evaluateKnights(const Board& board, Color color){
    int colorIndex = static_cast<int>(color);
    Bitboard knightBits = board.pieces(PieceType::KNIGHT, color);
    int pc = static_cast<int>(PieceType::KNIGHT) * 2 + colorIndex;
    while(knightBits != 0){
        int sqrIndex = knightBits.pop();
        Square knightSquare = Square(sqrIndex);
        Bitboard controlledSquares = attacks::knight(knightSquare);
        incPestoValues(colorIndex, pc, sqrIndex);
        addAttackDefendValues(board, controlledSquares, color);
        addMobilityBonus(board, controlledSquares, color, false);
        addOutpostBonus(board, knightSquare, color);
    }
}

int evaluate_board(const Board& board, GameResult result, GameResultReason reason) {
    int evaluation = 0;
    initEval();
    if (reason == GameResultReason::CHECKMATE) {
        return board.sideToMove() == Color::WHITE
               ? NEGINF
               : INF;
    }
    if (result == GameResult::DRAW) {
        return 0;
    }
    Board::CastlingRights rights = board.castlingRights();
    if (rights.has(Color::WHITE)) {
        evaluation += castle_rights_bonus;
    }
    if (rights.has(Color::BLACK)) {
        evaluation -= castle_rights_bonus;
    }
    evaluation += evaluatePieces(board);
    return evaluation;
}

int endgameMateEval(const Board& board, int egPhase, int currEval){
    std::vector<Square> cornerSquares = {Square(0), Square(7), Square(63), Square(56)};
    std::vector<Square> centerSquares = {Square(27), Square(28), Square(35), Square(36)};
    Square whiteKingSquare = board.kingSq(Color::WHITE);
    Square blackKingSquare = board.kingSq(Color::BLACK);
    int minWhiteCornerDistance = 0;
    int minBlackCornerDistance = 0;
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

int evaluatePieces(const Board& board) {
    Color white = Color::WHITE;
    Color black = ~white;
    for(Color color : {white, black}){
        evaluatePawns(board, color);
        evaluateKnights(board, color);
        evaluateBlockedPieces(board, color);
        evaluateKing(board, color);

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