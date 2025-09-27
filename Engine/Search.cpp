#include "Board.h"
#include "Search.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Evaluator.h"

Search::Search(){
    
}

Move Search::findBestMove(Board& board, int depth) {
    MoveGenerator gen(board);
    int moveCount = 0;
    gen.generateLegalMoves(moves, moveCount, 0);

    int bestScore = INT_MIN;
    Move bestMove;

    for (int i = 0; i < moveCount; i++) {
        board.makeMove(moves[0][i]);
        int score = alphaBeta(board, depth - 1, INT_MIN, INT_MAX, !board.whiteToMove);
        board.unmakeMove(moves[0][i]);

        if (score > bestScore) {
            bestScore = score;
            bestMove = moves[0][i];
        }
    }
    return bestMove;
}



int Search::alphaBeta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0) {
        return Evaluator::evaluate(board);
    }

    MoveGenerator gen(board);
    int moveCount = 0;
    gen.generateLegalMoves(moves, moveCount, depth);

    // If no legal moves → checkmate or stalemate
    if (moveCount == 0) {
        // Convention: high negative if checkmated, 0 for stalemate
        if (gen.isSquareAttacked(board.getKingPosition(board.whiteToMove ? white : black), board.whiteToMove ? black : white) ){
            return maximizingPlayer ? -100000 : 100000; 
        }
        return 0; 
    }

    if (maximizingPlayer) {
        int value = INT_MIN;
        for (int i = 0; i < moveCount; i++) {
            board.makeMove(moves[depth][i]);

            int childValue = alphaBeta(board, depth - 1, alpha, beta, false);

            board.unmakeMove(moves[depth][i]);

            value = std::max(value, childValue);
            alpha = std::max(alpha, value);
            if (alpha >= beta) {
                break; // beta cutoff
            }
        }
        return value;
    } else {
        int value = INT_MAX;
        for (int i = 0; i < moveCount; i++) {
            board.makeMove(moves[depth][i]);

            int childValue = alphaBeta(board, depth - 1, alpha, beta, true);

            board.unmakeMove(moves[depth][i]);

            value = std::min(value, childValue);
            beta = std::min(beta, value);
            if (beta <= alpha) {
                break; // alpha cutoff
            }
        }
        return value;
    }
}
