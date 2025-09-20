
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


int main()
{
    MoveGenerator::initKnightAttacks();
    MoveGenerator::initKingAttacks();
    MoveGenerator::initSlidingAttacks();


    Board board = Board();
    board.setTestingPosition();
    board.print();
    MoveGenerator gen(board);
    std::vector<Move> moves = {};
    gen.generateQueenMoves(moves);
    
    std::cout << "Size" << moves.size() << std::endl;
    for (Move move : moves){
        std::cout << move.toString() << std::endl;
    }

    
    

}
