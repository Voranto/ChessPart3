#include "Move.h"
#include "pieceColor.h"
#include "PieceType.h"

Move::Move(PieceType type, PieceColor color, std::pair<int, int> from, std::pair<int, int> to) {

	this->pieceType = type;
	this->pieceColor = color;
	this->from = from;
	this->to = to;
	this->eats = std::nullopt;
}




std::ostream& operator<<(std::ostream& os, const Move& mv) {

	os << pieceColorNames[mv.pieceColor] << " " << pieceTypeNames[mv.pieceType];
	os << " moves from: (" << std::get<0>(mv.from) <<"," << std::get<1>(mv.from);
	os << ") to: (" << std::get<0>(mv.to ) << "," << std::get<1>(mv.to) << ")";
	if (mv.eats.has_value()) {
		os << " and eats: " << mv.eats.value();
	}

    return os;
}

bool Move::operator<(const Move& other) const {
	return std::tie(from, to, pieceType, pieceColor) < std::tie(other.from, other.to, other.pieceType, other.pieceColor);
}
bool Move::operator==(const Move& other) const {
	return this->from == other.from && this->to == other.to && this->pieceColor == other.pieceColor && this->pieceType == other.pieceType && this->eats == other.eats;
}

bool Move::equals(Move move) {

	if (move.from == this->from && move.to == this->to && move.pieceColor == this->pieceColor && move.pieceType == this->pieceType) {
		return true;
	}
	return false;
	
}

std::string Move::toAlgebraic(std::pair<int, int> pos) {
	char file = 'a' + pos.first;          // column to letter
	char rank = '8' - pos.second;           // row to number
	return std::string() + file + rank;
}



std::string Move::toString() {
	return toAlgebraic(from) + toAlgebraic(to);
}
