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
    std::optional<PieceType> promotesTo;

	Move(PieceType type, PieceColor color, std::pair<int, int> from, std::pair<int, int> to);

	friend std::ostream& operator<<(std::ostream& os, const Move& mv);

	bool operator<(const Move& other) const;

	bool operator==(const Move& other) const;


    Move(Move&& other) noexcept
        : pieceType(other.pieceType),
        pieceColor(other.pieceColor),
        from(std::move(other.from)),
        to(std::move(other.to)),
        eats(std::move(other.eats)) // Move the optional member
    {
        
    }

    // Move assignment operator
    Move& operator=(Move&& other) noexcept
    {
        if (this != &other) {
            pieceType = other.pieceType;
            pieceColor = other.pieceColor;
            from = std::move(other.from);
            to = std::move(other.to);
            eats = std::move(other.eats);


        }
        return *this;
    }

    Move(const Move& other)
        : pieceType(other.pieceType),
        pieceColor(other.pieceColor),
        from(other.from),
        to(other.to),
        eats(other.eats) // Copy the optional<Piece> directly
    {}

    Move& operator=(const Move& other) {
        if (this != &other) {  // Self-assignment check
            pieceType = other.pieceType;
            pieceColor = other.pieceColor;
            from = other.from;
            to = other.to;
            eats = other.eats;
        }
        return *this;
    }

	Move() = default;
	bool equals(Move move);
    std::string toString();
private:
    std::string toAlgebraic(std::pair<int, int> pos);
    
    
};

