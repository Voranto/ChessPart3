#include "Board.h"
#include <cstdint>
#include <random>
#include <iostream>
#include "MoveGenerator.h"
uint64_t ZobristTable[12][64]; // 12 piece types 64 squares
uint64_t ZobristSide;          // Side to move
uint64_t ZobristCastling[16];  // Castling rights states
uint64_t ZobristEnPassant[8];  // En passant file

int pieceIndex(PieceType type, PieceColor color) {
    int base = (color == white) ? 0 : 6;
    return base + static_cast<int>(type); // assuming Pawn=0..King=5
}

int castlingRightsAfterMove(const Move& move, int oldCastling){
	int newCastling = oldCastling;
// Moving king removes both rights for that color
    if (move.pieceType == King) {
        if (move.pieceColor == white)
            newCastling &= 0b1100; // remove white K+Q
        else
            newCastling &= 0b0011; // remove black K+Q
    }

    // Moving rooks removes relevant side
    if (move.pieceType == Rook) {
        if (move.pieceColor == white) {
            if (move.from == 0) newCastling &= 0b1110; // a1 rook, remove Q
            if (move.from == 7) newCastling &= 0b1101; // h1 rook, remove K
        } else {
            if (move.from == 56) newCastling &= 0b0111; // a8 rook, remove q
            if (move.from == 63) newCastling &= 0b1011; // h8 rook, remove k
        }
    }
	return newCastling;
}


Board::Board() {
	


	blackPawns = 0;
	blackRooks = 0;
	blackBishops = 0;
	blackKing = 0;
	blackQueens = 0;
	blackKnights = 0;
	
	whitePawns = 0;
	whiteKing = 0;
	whiteRooks = 0;
	whiteBishops = 0;
	whiteKnights = 0;
	whiteQueens = 0;

	whiteToMove = true;
	castlingRights = 15;

	halfMoveClock = 0;
	fullMoveNumber = 0;

	enPassantSquare = -1;

	zobristHash = 0;

	this->initZobristKeys();
	
	
	history = {};
}

void Board::initZobristKeys() {
	std::mt19937_64 rng(0xDEADBEEF); // fixed seed for reproducibility
	std::uniform_int_distribution<uint64_t> dist;

	// Fill piece-square table
	for (int piece = 0; piece < 12; ++piece) {
		for (int square = 0; square < 64; ++square) {
			ZobristTable[piece][square] = dist(rng);
		}
	}

	// Side to move
	ZobristSide = dist(rng);

	// Castling rights (16 possibilities)
	for (int i = 0; i < 16; ++i) {
		ZobristCastling[i] = dist(rng);
	}

	// En passant files
	for (int file = 0; file < 8; ++file) {
		ZobristEnPassant[file] = dist(rng);
	}
}


//NOTE: HAS TO BE CALLED BEFORE EXECUTING MOVE
void Board::updateZobrist(const Move& move) {
    // 1️⃣ Remove moving piece from old square
    int idx = pieceIndex(move.pieceType, move.pieceColor);
    zobristHash ^= ZobristTable[idx][move.from];

    // 2️⃣ Add moving piece to new square
    zobristHash ^= ZobristTable[idx][move.to];

    // 3️⃣ If capture, remove captured piece
    if (move.pieceEatenType != None) {
        PieceColor enemyColor = (move.pieceColor == white) ? black : white;
        int capIdx = pieceIndex(move.pieceEatenType, enemyColor);
        zobristHash ^= ZobristTable[capIdx][move.to];
    }

    // 4️⃣ Handle promotion
    if (move.promotionPiece != None) {
        // Remove pawn from destination
        int pawnIdx = pieceIndex(Pawn, move.pieceColor);
        zobristHash ^= ZobristTable[pawnIdx][move.to];

        // Add promoted piece
        int promoIdx = pieceIndex(move.promotionPiece, move.pieceColor);
        zobristHash ^= ZobristTable[promoIdx][move.to];
    }

    // 5️⃣ En passant
    if (move.isEnPassant) {
        int epSquare = (move.pieceColor == white) ? move.to - 8 : move.to + 8;
        int epPawnIdx = pieceIndex(Pawn, (move.pieceColor == white) ? black : white);
        zobristHash ^= ZobristTable[epPawnIdx][epSquare];
    }


	int newCastling = castlingRightsAfterMove(move,this->castlingRights);

    

	zobristHash ^= ZobristCastling[this->castlingRights];
	zobristHash ^= ZobristCastling[newCastling];


    // 6️⃣ Side to move
    zobristHash ^= ZobristSide;
}

void Board::setStartingPosition() {
	whiteToMove = true;

	//setting up board (LITTLE ENDIAN)
	whitePawns = 0x000000000000FF00;
	whiteRooks = 0x0000000000000081;
	whiteKnights = 0x0000000000000042;
	whiteBishops = 0x0000000000000024;
	whiteQueens = 0x0000000000000008;
	whiteKing = 0x0000000000000010;

	blackPawns = 0x00FF000000000000;
	blackRooks = 0x8100000000000000;
	blackBishops = 0x2400000000000000;
	blackKnights = 0x4200000000000000;
	blackQueens = 0x0800000000000000;
	blackKing = 0x1000000000000000;

	whitePieces = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
	blackPieces = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
	allPieces = whitePieces | blackPieces;

}

//helper functions
char Board::getLetterOfPieceType(PieceType type) {
	if (type == None) { return '.'; }
	if (type == Knight){return 'N';}
	return pieceTypeNames[type][0];
}


//using little endian, bit 0 is a1
std::pair<PieceType, PieceColor> Board::getPieceTypeAtBit( int bit) {
	uint64_t mask = 1ULL << bit;
	if ((blackPawns & mask) != 0) {
		return { Pawn,black };
	}
	if ((blackRooks & mask) != 0) {
		return { Rook,black };
	}
	if ((blackQueens & mask) != 0) {
		return { Queen,black };
	}
	if ((blackBishops & mask) != 0) {
		return { Bishop,black };
	}
	if ((blackKnights & mask) != 0) {
		return { Knight,black };
	}
	if ((blackKing & mask) != 0) {
		return { King,black };
	}
	if ((whitePawns & mask) != 0) {
		return { Pawn,white };
	}
	if ((whiteRooks & mask) != 0) {
		return { Rook,white };
	}
	if ((whiteQueens & mask) != 0) {
		return { Queen,white };
	}
	if ((whiteBishops & mask) != 0) {
		return { Bishop,white };
	}
	if ((whiteKnights & mask) != 0) {
		return { Knight,white };
	}
	if ((whiteKing & mask) != 0) {
		return { King,white };
	}

	return { None,white };
}