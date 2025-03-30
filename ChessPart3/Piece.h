#pragma once
#include "pieceColor.h"
#include "PieceType.h"
#include <tuple>
#include <iostream>

class Piece
{
public:
	std::pair<int, int> pos;

	PieceColor color;

	PieceType type;

	Piece(int x, int y, PieceColor color, PieceType type);
	Piece(std::pair<int,int> pos, PieceColor color, PieceType type);
	Piece();

	friend std::ostream& operator<<(std::ostream& os, const Piece& pc);

	bool operator==(const Piece& other) const;


};

