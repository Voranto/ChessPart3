
#include <iostream>
#include <optional>
/*
#include "GUI.h"
#include "Button.h"
#include "GUI_Screens.h"
#include "ChessGUI.h"
#include "MultiplayerChessGUI.h"
#include "TextBox.h"
*/
#include "Engine/Board.h"
#include <bitset>
#include <chrono>
#include <thread>
#include <math.h>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include "Engine/MoveGenerator.h"
#include <bits/stdc++.h>
#include <chrono>
#include "Engine/Search.h"
#include "Engine/Evaluator.h"
#include "Engine/TTEntry.h"

void printBitboard(uint64_t board) {
    std::bitset<64> bits(board);
    for (int rank = 7; rank >= 0; --rank) { // rank 8 to 1
        for (int file = 0; file < 8; ++file) { // file a to h
            int sq = rank * 8 + file; 
            std::cout << bits[sq] << " ";
        }
        std::cout << std::endl;
    }
}

bool comp(Move a, Move b){
    return a.toString() < b.toString();
}


int main()
{
    MoveGenerator::initKnightAttacks();
    MoveGenerator::initKingAttacks();
    MoveGenerator::initSlidingAttacks();
    MoveGenerator::initPawnAttacks();
    Search::initOpeningTree();

    
    Board board = Board();
    board.setStartingPosition();
    board.enPassantSquare = -1;
    board.print();
    
    

    MoveGenerator gen(board);

    
    int moveCount = 0;

    int choice = 0;


    gen.generateLegalMoves(moves,moveCount,0);


    int c = 0;
    int total = 0;
    int ans ;
    using namespace std::chrono;
    




    for (int i = 1; i < MAX_DEPTH; i++){
        auto start = high_resolution_clock::now();

        Search search = Search();

        Move bestMove = search.findBestMove(board,i );
        std::cout << "DEPTH: " << i << std::endl;
        std::cout <<"BEST MOVE: " <<  bestMove.toString() << std::endl;

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<seconds>(end - start);

        std::cout << "Time taken: " << duration.count() << " seconds\n";
        std::cout << "-------------------------" << std::endl;
    }
        
}
