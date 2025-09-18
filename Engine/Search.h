#include "Move.h"
#include "Board.h"
#pragma once

class Search{
	public: 
		Move findBestMove(Board& board, int depth);

	private:
		int alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);

};
