#include "pch.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include "gtest/gtest.h"
#include "Chessboard.h"
#include "GUI.h"
#include "Button.h"
#include "GUI_Screens.h"
#include "ChessGUI.h"
#include "PieceColor.h"

TEST(ChessboardTest, NobodyInCheck) {
	Chessboard board("rnbqkbnr/pppp1ppp/8/4p3/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
	EXPECT_TRUE(!board.isInCheck(white) && !board.isInCheck(black));
}

TEST(ChessboardTest, IsInCheckWhite) {
	Chessboard board("rnb1kbnr/ppppqppp/8/8/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 3");
	EXPECT_TRUE(board.isInCheck(white));
}

TEST(ChessboardTest, IsInCheckBlack) {
	Chessboard board("rnbqkbnr/ppppp1pp/8/5p1Q/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
	EXPECT_TRUE(board.isInCheck(black));
}

TEST(ChessboardTest, ExecuteAndUndoSimpleMove) {
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	Chessboard board(startingFEN);

	Move move(Pawn, white, std::make_pair(4,6), std::make_pair(4, 4));

	board.executeMove(move);
	board.undoMove(move);
	EXPECT_TRUE(board.getFEN() == startingFEN);
	std::cout << board.getFEN() << std::endl;
	std::cout << startingFEN << std::endl;

}

TEST(ChessboardTest, ExecuteAndUndoEnPassant) {
    // Position where white pawn can capture black pawn
    Chessboard board("rnbqkbnr/pppppppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    std::cout << board.enPassantSquare.value().first << " " << board.enPassantSquare.value().second << std::endl;
    
    Move captureMove(PieceType::Pawn, PieceColor::black, { 3,4 }, { 4,5 });

    captureMove.eats = board.eats(captureMove);
    board.executeMove(captureMove);
    EXPECT_TRUE(board.getFEN() == "rnbqkbnr/pppppppp/8/8/8/4p3/PPPP1PPP/RNBQKBNR w KQkq - 0 2");
    std::cout << board.getFEN() << std::endl;

    board.undoMove(captureMove);


    EXPECT_TRUE(board.getFEN() == "rnbqkbnr/pppppppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    std::cout << board.getFEN() << std::endl;

}

TEST(ChessboardTest, ExecuteAndUndoCastlingMove) {
    // Start with a position where castling is legal
    std::string startingFEN = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    Chessboard board(startingFEN);

    // White kingside castle (e1 to g1)
    Move castleMove(PieceType::King, PieceColor::white, { 4,7 }, { 6,7 });

    board.executeMove(castleMove);
    EXPECT_TRUE(board.getFEN() == "r3k2r/8/8/8/8/8/8/R4RK1 b --kq - 1 1");

    board.undoMove(castleMove);
    std::cout << board.getFEN();

    EXPECT_TRUE(board.getFEN() == startingFEN);
}

TEST(ChessboardTest, CountMoves) {
    std::string startingFEN = "2bqk2r/ppppp1pp/1r3n2/1bn2p2/2P1P3/1QN2N2/PP1P1PPP/R1B1KB1R w KQk- - 0 1";
    Chessboard board(startingFEN);
    std::vector<int> amountOfMoves = { 34,1237,41999, 1503852 };

    for (int i = 1; i <= 4; i++) {
        int val = board.countMoves(i);
        EXPECT_TRUE(val == amountOfMoves[i - 1]) << "Expected: " << amountOfMoves[i - 1] << std::endl << "Result: " << val << std::endl;
    }
}

TEST(ChessboardTest, PromotionTest) {
    std::string startingFEN = "k7/7P/8/8/8/8/8/R1B1KB1R w ---- - 0 1";
    Chessboard board(startingFEN);

    board.calculatePawnMoves(white);

    EXPECT_TRUE(board.whiteMovesCount == 4) << board.whiteMovesCount; 
}