#pragma once
#include "PieceType.h"
#include "pieceColor.h"
#include <tuple>
#include <iostream>
#include <optional>

class Move
{

public:
	PieceType pieceType;
	PieceColor pieceColor;

	int from;
    int to;

    PieceType promotionPiece;
    PieceType pieceEatenType;

    bool isEnPassant;

    Move(PieceType type, PieceColor color, int from, int to, PieceType promotionPiece = None,
        PieceType pieceEatenType = None, bool isEnPassant = false);

    std::string toString() const {
        // Convert to algebraic notation like "e2e4" or "e7e8Q"
        std::string s;
        s += squareToString(this->from);
        s += squareToString(this->to);
        return s;
    }

    private:
        std::string squareToString(int square) const {
            char file = 'a' + (square % 8);
            char rank = '1' + (square / 8);
            return { file, rank };
        }
};

