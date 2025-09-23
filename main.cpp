
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

    Board board = Board();
    board.setStartingPosition();
    
    
    MoveGenerator gen(board);
    std::vector<Move> moves = {};
    std::vector<Move> movesDone = {};
    int choice = 0;
    
    board.print();
    moves.clear();
    gen.generateLegalMoves(moves);

    

    std::sort(moves.begin(),moves.end(),comp);

    std::cout << "Size" << moves.size() << std::endl;
    int c = 0;
    int total = 0;
    int ans ;
    for (Move move : moves){
        board.makeMove(move);
        ans = board.countMoves(5);
        std::cout << c << ". "<< move.toString()  << ":" << ans << std::endl;
        total+= ans;
        c++;
        board.unmakeMove(move);
    }
    std::cout << total << std::endl;
    
        
}
