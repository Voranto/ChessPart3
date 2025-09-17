#include "Board.h"
#include "Move.h"
#include <vector>
#include <cstdint>

#pragma once


class MoveGenerator{
	public:
		explicit MoveGenerator(const Board& board);

		void generateLegalMoves(std::vector<Move>& moves) const;
		void generatePseudoLegalMoves(std::vector<Move>& moves) const;
    		void generateCaptures(std::vector<Move>& moves) const;


	private:
    		const Board& board;

    // Piece-specific helpers
    		void generatePawnMoves(std::vector<Move>& moves) const;
    		void generateKnightMoves(std::vector<Move>& moves) const;
    		void generateBishopMoves(std::vector<Move>& moves) const;
    		void generateRookMoves(std::vector<Move>& moves) const;
    		void generateQueenMoves(std::vector<Move>& moves) const;
    		void generateKingMoves(std::vector<Move>& moves) const;

    		// Legality check
    		bool isLegal(const Move& move) const;

    		// Attack utilities
    		uint64_t attacksFromSquare(int square, PieceType piece) const;
};
};
