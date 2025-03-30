#pragma once
#include "PieceType.h"
#include "pieceColor.h"
#include <tuple>
#include <iostream>
#include <optional>
#include "Piece.h"

class Move
{

public:
	PieceType pieceType;
	PieceColor pieceColor;

	std::pair<int, int> from;
	std::pair<int, int> to;

	std::optional<Piece> eats;

	Move(PieceType type, PieceColor color, std::pair<int, int> from, std::pair<int, int> to);

	friend std::ostream& operator<<(std::ostream& os, const Move& mv);

	bool operator<(const Move& other) const;

	bool operator==(const Move& other) const;

	Move();
	bool equals(Move move);
};

