#include "Board.h"

#pragma once
constexpr int PIECE_VALUES[6] = {
    100,  // Pawn
    320,  // Knight
    330,  // Bishop
    500,  // Rook
    900,  // Queen
    20000 // King (arbitrary large)
};



class Evaluator{
	public:
		static int evaluate(const Board& board);
};
