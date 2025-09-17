#include <vector>
#include <iostream>


#pragma once


struct BoardState{
	int castlingRights;
	int enPassantSquare;
	int halfMoveClock;
	uint64_t zobristHash;
	int capturedPiece;
}

class Board{
public:
	uint64_t blackPawns, blackRooks, blackBishops, blackKnights, blackQueens, blackKing;

	uint64_t whitePawns, whiteRooks, whiteBishops, whiteKnights, whiteQueens, whiteKing;
	
	//aggregate bitboards
	uint64_t whitePieces;
	uint64_t blackPieces;
	uint64_t allPieces;

	//GameState
	bool whiteToMove;
	//1 = WK, 2 = WQ, 4 = BK, 8 = BQ
	int castlingRights;
	int halfMoveClock;
	int fullMoveNumber;
	int enPassantSquare;

	uint64_t zobristHash;

	std::vector<BoardState> history;
	
	Board();

	void setStartingPosition();
	void makeMove(const Move& move);
	void unmakeMove(const Move& move);
	bool isSquareAttacked(int square, bool byWhite) const;

	void print() const;

	void initZobristKeys();
	void updateZobrist(const Move& move);


};
