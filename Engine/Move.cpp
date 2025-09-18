#include "Move.h"
#include "pieceColor.h"
#include "PieceType.h"

Move::Move(PieceType type, PieceColor color, int from, int to, PieceType promotionPiece,
	PieceType pieceEatenType, bool isEnPassant ) {

	this->pieceType = type;
	this->pieceColor = color;
	this->from = from;
	this->to = to;
	this->pieceEatenType = pieceEatenType;
	this->isEnPassant = isEnPassant;
	this->promotionPiece = promotionPiece;
}