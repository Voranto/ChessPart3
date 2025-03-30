#include <vector>
#include <optional>
#include "pieceColor.h"
#include "Square.h"
#include <string>
#include <iostream>
#include <tuple>
#include <bitset>
#include "Move.h"
#include <set>
#include "Piece.h"
#include <map>

#pragma once
class Chessboard
{
public:
	
	
	std::string FEN;

	uint64_t blackPawns = 0;
	uint64_t blackRooks = 0;
	uint64_t blackBishops = 0;
	uint64_t blackKnights = 0;
	uint64_t blackQueens = 0;
	uint64_t blackKing = 0;

	uint64_t whitePawns = 0;
	uint64_t whiteRooks = 0;
	uint64_t whiteBishops = 0;
	uint64_t whiteKnights = 0;
	uint64_t whiteQueens = 0;
	uint64_t whiteKing = 0;

	PieceColor toMove;

	int fullMoves; 
	int halfMoves;

	std::optional<std::pair<int,int>> enPassantSquare;

	std::optional<std::pair<int, int>> oldEnPassantSquare;

	bool whiteKingCastling;
	bool whiteQueenCastling;
	bool blackKingCastling;
	bool blackQueenCastling;

	std::vector<Move> whiteMoves;
	std::vector<Move> blackMoves;

	std::vector<Move> oldWhiteMoves;
	std::vector<Move> oldBlackMoves;

	Chessboard(std::string FEN);
	Chessboard();

	void setFEN(std::string FEN);

	std::string getFEN();

	uint64_t getCombinedBoard(PieceColor color);

	static void printBitset(std::bitset<64> bits);

	friend std::ostream& operator<<(std::ostream& os, const Chessboard& cb);

	void calculateMoves(bool calculateExtended = true);

	void calculateWhiteMoves(bool calculateExtended = true);

	void calculateBlackMoves(bool calculateExtended = true);

	void calculateKnightMoves(PieceColor color, bool calculateExtended = true);

	void calculatePawnMoves(PieceColor color, bool calculateExtended = true);

	void calculateBishopMoves(PieceColor color, bool calculateExtended = true);

	void calculateRookMoves(PieceColor color, bool calculateExtended = true);

	void calculateQueenMoves(PieceColor color, bool calculateExtended = true);

	void calculateKingMoves(PieceColor color, bool calculateExtended = true);

	std::string convertIntToBinaryString(uint64_t val);

	std::pair<int, int> convertGridCoords(int val);

	int convertGridCoords(std::pair<int, int>);

	void executeMove(Move& move);

	void undoMove(Move& move);
	
	uint64_t* getBoardOfType(PieceType type,PieceColor color);

	std::vector<int> getBitIndexes(uint64_t n);

	bool wouldMoveCauseCheck(Move& move);

	std::pair<int, int> getKingPosition(PieceColor color);

	std::map<PieceType, int> getMaterial(PieceColor color);

	int simpleEvaluation();

	Move calculateBestMove();

	int negaMax(int depth);

	int negaMaxAlphaPruning(int depth, int alpha, int beta);

	std::optional<Piece> eats(Move& mv);

	int countMoves(int depth);

	void storeOldMoves();

	void restoreOldMoves();

	void storeEnPassantSquare();

	void restoreEnPassantSquare();
};

