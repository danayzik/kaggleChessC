
#ifndef CHESSBOT_UTILITIES_H
#define CHESSBOT_UTILITIES_H
#include "chess.hpp"
#include "transposition_table.h"
using namespace chess;
using namespace std;
using namespace transpositions;
namespace bot_utils {
    static constexpr int MAX_DEPTH = 40;
    static constexpr int INF = 32767;
    static constexpr int NEGINF = -32768;
    static constexpr uint64_t allOnes = ~0ULL;
    static constexpr Bitboard fullBitboard = Bitboard(allOnes);
    static vector <PieceType> pieceTypes = {
            PieceType::PAWN,
            PieceType::KNIGHT,
            PieceType::BISHOP,
            PieceType::ROOK,
            PieceType::QUEEN,
            PieceType::KING
    };
    constexpr int16_t materialValue[6] = {
            10,  // Pawn
            30,  // Knight
            32,  // Bishop
            50,  // Rook
            90,  // Queen
            0     // King
    };

    inline Bitboard tripleFileMask(const Bitboard& fileBits,const File& f){
        Bitboard leftFile = f >> std::min(1, (int)f);
        Bitboard rightFile = f << std::min(1, (int)f-7);
        return  (leftFile | rightFile | fileBits);
    }

    inline Bitboard sideFilesMask(const Bitboard& fileBits,const File& f){
        Bitboard tripleMask = tripleFileMask(fileBits, f);
        return tripleMask & (~fileBits);
    }
    inline Bitboard getRanksInfront(int pieceRank, bool isWhite){
        return isWhite? fullBitboard << 8*(pieceRank + 1): fullBitboard >> 8*(8-pieceRank);
    }

    inline bool isCheckMove(Board& board, const Move& move) {
        board.makeMove(move);
        bool isCheck = board.inCheck();
        board.unmakeMove(move);
        return isCheck;
    }
    inline void setCheckAndHashMove(Board& board, Move& move, int currDepth, TranspositionTable* tt){
        board.makeMove(move);
        move.isCheck = board.inCheck();
        uint64_t zobrist = board.hash();
        move.isHash = tt->isHashedPosition(zobrist, currDepth);
        board.unmakeMove(move);
    }

    inline pair<GameResultReason, GameResult> isGameOver(const Board& board, const Movelist& legalMovelist)  {
        if (board.isHalfMoveDraw()) return board.getHalfMoveDrawType();
        if (board.isInsufficientMaterial()) return {GameResultReason::INSUFFICIENT_MATERIAL, GameResult::DRAW};
        if (board.isRepetition()) return {GameResultReason::THREEFOLD_REPETITION, GameResult::DRAW};
        if (legalMovelist.empty()) {
            if (board.inCheck()) return {GameResultReason::CHECKMATE, GameResult::LOSE};
            return {GameResultReason::STALEMATE, GameResult::DRAW};
        }

        return {GameResultReason::NONE, GameResult::NONE};
    }

}

#endif
