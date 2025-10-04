#include "Piece.h"


Piece::Piece(int x, int y, PieceColor color, PieceType type) {
	this->pos = std::make_pair(x, y);
	this->color = color;
	this->type = type;
}

Piece::Piece(std::pair<int, int> pos, PieceColor color, PieceType type) {
	this->pos = pos;
	this->color = color;
	this->type = type;
}

Piece::Piece(){}


std::ostream& operator<<(std::ostream& os, const Piece& pc){
	os << pieceColorNames[pc.color] << " ";
	os << pieceTypeNames[pc.type] << " ";
	os << "in " << std::get<0>(pc.pos) << " " << std::get<1>(pc.pos);
	return os;
}

bool Piece::operator==(const Piece& other) const {
	return this->pos == other.pos && this->color == other.color && this->type == other.type;
}