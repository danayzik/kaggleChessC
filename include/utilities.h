
#ifndef CHESSBOT_UTILITIES_H
#define CHESSBOT_UTILITIES_H
#include "chess.hpp"
#include <map>
using namespace chess;
using namespace std;
namespace bot_utils {
    static constexpr int INF = 32767;
    static constexpr int NEGINF = -32768;

    static vector <PieceType> pieceTypes = {
            PieceType::PAWN,
            PieceType::KNIGHT,
            PieceType::BISHOP,
            PieceType::ROOK,
            PieceType::QUEEN,
            PieceType::KING
    };

    inline Bitboard tripleFileMask(Bitboard fileBits, File f){
        Bitboard leftFile = f >> std::min(1, (int)f);
        Bitboard rightFile = f >> std::min(1, (int)f-7);
        return  (leftFile | rightFile | fileBits);
    }

    inline Bitboard sideFilesMask(Bitboard fileBits, File f){
        Bitboard tripleMask = tripleFileMask(fileBits, f);
        return tripleMask & (~fileBits);
    }
    inline Bitboard getRanksInfront(int pieceRank, bool isWhite){
        uint64_t allOnes = ~0ULL;
        Bitboard full = Bitboard(allOnes);

        Bitboard ranksInfrontMask = isWhite? full << 8*(pieceRank + 1): full >> 8*(8-pieceRank);
        return ranksInfrontMask;
    }

}

#endif
