#include "Board.h"
#include "Search.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "TTEntry.h"
Search::Search(){
    
}

void Search::initOpeningTree(){
    std::ifstream file("high_elo_openings.csv");
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int column = 0;

        while (std::getline(ss, cell, ',')) {
            column++;
            if (column == 10){
                std::cout << cell << std::endl;
            }
        }

    }

    file.close();

}

Move Search::findBestMove(Board& board, int depth) {
    clearTT();
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
    int alphaOrig = alpha;
    uint64_t key = board.zobristHash;

    // 1️⃣ TT probe
    if (auto* entry = probeTT(key)) {
        if (entry->depth >= depth) {
            if (entry->flag == EXACT) return entry->score;
            if (entry->flag == LOWERBOUND && entry->score >= beta) return entry->score;
            if (entry->flag == UPPERBOUND && entry->score <= alpha) return entry->score;
            
        }
    }
    
    
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

    Move bestMove;
    if (auto* entry = probeTT(key)) bestMove = entry->bestMove;

    std::sort(moves[depth], moves[depth] + moveCount, [](const Move& a, const Move& b) {
        int scoreA = 0, scoreB = 0;
        if (a.pieceEatenType != None) scoreA = PIECE_VALUES[a.pieceEatenType]- PIECE_VALUES[a.pieceType];
        if (b.pieceEatenType != None) scoreB = PIECE_VALUES[b.pieceEatenType] - PIECE_VALUES[b.pieceType];
        if (a.promotionPiece != None) scoreA += 1000;
        if (b.promotionPiece != None) scoreB += 1000;
        return scoreA > scoreB; // higher-score first
    });

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
        TTFlag flag;
        if (value <= alphaOrig) flag = UPPERBOUND;
        else if (value >= beta) flag = LOWERBOUND;
        else flag = EXACT;

        storeTT(key, depth, value, flag, bestMove);
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

        TTFlag flag;
        if (value <= alphaOrig) flag = UPPERBOUND;
        else if (value >= beta) flag = LOWERBOUND;
        else flag = EXACT;

        storeTT(key, depth, value, flag, bestMove);
        return value;
    }
}
