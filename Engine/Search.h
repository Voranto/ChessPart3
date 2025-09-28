#include "Move.h"
#include "Board.h"
#include "MoveTree.h"
#pragma once

class Search{
	public: 
		static MoveTree openingTree;
		static void initOpeningTree();
		
		Search();

		Move findBestMove(Board& board, int depth);

	private:
		int alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);

};
