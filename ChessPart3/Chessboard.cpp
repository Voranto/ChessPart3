
#include "Chessboard.h"
#include "pieceColor.h"
#include <bitset>
#include <cmath>
#include <intrin.h>
#include <map>
#include <cstdint>
#include <future>
#include <array>
#include <sstream>
#include <ctype.h>



const int height = 8;
const int width = 8;
const int kingWeight = INT_MAX;
const int queenWeight = 1000;
const int rookWeight = 525;
const int bishopWeight = 350;
const int knightWeight = 350;
const int pawnWeight = 100;
const int DEPTH = 3;
const int MIN_SCORE = -99999;
const int MAX_SCORE = 99999;


static int maxThreads = std::thread::hardware_concurrency();


Chessboard::Chessboard() {}

Chessboard::Chessboard(std::string FEN) {
	this->setFEN(FEN);
	this->history;
}

void Chessboard::reset() {
	// Clear bitboards
	blackPawns = blackRooks = blackBishops = blackKnights = blackQueens = blackKing = 0;
	whitePawns = whiteRooks = whiteBishops = whiteKnights = whiteQueens = whiteKing = 0;

	// Reset side to move
	toMove = white;

	// Reset move counters
	fullMoves = 1;
	halfMoves = 0;

	// Clear en passant square
	enPassantSquare.reset();

	// Reset castling rights
	whiteKingCastling = false;
	whiteQueenCastling = false;
	blackKingCastling = false;
	blackQueenCastling = false;

	// Clear move history
	history.clear();

	// Clear legal move arrays
	whiteMovesCount = 0;
	blackMovesCount = 0;
	whiteMoves.fill(Move());
	blackMoves.fill(Move());
}

void Chessboard::setFEN(std::string FEN) {
	this->FEN = FEN;
	this->reset();

	int n = FEN.size();
	int currentX = 0, currentY = 0;
	int i;
	for (i = 0; i < n; i++) {
		char currentChar = FEN[i];
		if (currentChar == ' ') {
			i++;
			break;
		}
		else if (currentChar == '/') {
			currentY++;
			currentX = 0;
			continue;
		}
		if (isdigit(currentChar)) {
			currentX += currentChar - '0';
		}
		else {
			int oneDimensionalIndex = currentY * 8 + currentX;

			switch (currentChar) {
			case 'p': this->blackPawns |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'r': this->blackRooks |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'b': this->blackBishops |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'n': this->blackKnights |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'k': this->blackKing |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'q': this->blackQueens |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'P': this->whitePawns |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'R': this->whiteRooks |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'B': this->whiteBishops |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'N': this->whiteKnights |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'K': this->whiteKing |= (1ULL << (63 - oneDimensionalIndex)); break;
			case 'Q': this->whiteQueens |= (1ULL << (63 - oneDimensionalIndex)); break;
			}
			currentX++;
		}
	}
	
	// Now process metadata
	if (i < n) { this->toMove = (FEN[i] == 'w') ? PieceColor(white) : PieceColor(black);}
	i+=2;
	if (i < n) {
		this->whiteKingCastling = (FEN[i++] == 'K');
		this->whiteQueenCastling = FEN[i++] == 'Q';
		this->blackKingCastling = FEN[i++] == 'k';
		this->blackQueenCastling = FEN[i++] == 'q';

	}
	i+= 1;
	if (i < n && FEN[i] != '-') {
		if (i + 1 < n) { // Ensure we don’t access out-of-bounds
			this->enPassantSquare = std::make_pair(FEN[i] - 'a', 7 - (FEN[i + 1] - '1'));
			i++;
		}
		
	}
	i += 2;
	if (i < n) this->halfMoves = FEN[i] - '0';
	i += 2;
	if (i < n) this->fullMoves = FEN[i++] - '0';
}

std::ostream& operator<<(std::ostream& os, const Chessboard& cb) {

	os << "White Pawns: ";
	cb.printBitset(cb.whitePawns);
	os << "White knights: ";
	cb.printBitset(cb.whiteKnights);
	os << "White bishops: ";
	cb.printBitset(cb.whiteBishops);
	os << "White Rooks: ";
	cb.printBitset(cb.whiteRooks);
	os << "White Queens: ";
	cb.printBitset(cb.whiteQueens);
	os << "White king: ";
	cb.printBitset(cb.whiteKing);
	os << "Black Pawns: ";
	cb.printBitset(cb.blackPawns);
	os << "Black knights: ";
	cb.printBitset(cb.blackKnights);
	os << "Black bishops: ";
	cb.printBitset(cb.blackBishops);
	os << "Black Rooks: ";
	cb.printBitset(cb.blackRooks);
	os << "Black Queens: ";
	cb.printBitset(cb.blackQueens);
	os << "Black king: ";
	cb.printBitset(cb.blackKing);

	return os;
}

std::pair<int, int> Chessboard::convertGridCoords(int val) {
	if (val < 0 || val > 63) {
		std::cout << "ERROR: " << val << std::endl;
	}
	return std::make_pair(val % 8, val / 8);
}

int Chessboard::convertGridCoords(std::pair<int, int> val) {
	return val.second * 8 + val.first;
}

uint64_t Chessboard::getCombinedBoard(PieceColor color) {
	if (color == PieceColor(white)) {
		return whitePawns | whiteBishops | whiteKing | whiteQueens | whiteRooks | whiteKnights;
	}
	else {
		return blackPawns | blackBishops | blackKing | blackQueens | blackRooks | blackKnights;
	}

}


std::string Chessboard::convertIntToBinaryString(uint64_t val) {
	std::string answer = std::bitset<64>(val).to_string();


	return answer;
}

void Chessboard::calculateMoves(bool calculateExtended) {

	this->calculateBlackMoves(calculateExtended);
	this->calculateWhiteMoves(calculateExtended);
}

void  Chessboard::printBitset(std::bitset<64> bits) {
	std::string bitString = bits.to_string(); // Convert to string

	// Print in chunks of 8 bits
	for (size_t i = 0; i < bitString.size(); i++) {
		std::cout << bitString[i];

		// Add a space after every 8 bits
		if ((i + 1) % 8 == 0) {
			std::cout << " ";
		}
	}

	std::cout << std::endl;
}

void Chessboard::calculateWhiteMoves(bool calculateExtended) {
	this->whiteMovesCount = 0;

	this->calculateBishopMoves(PieceColor(white), calculateExtended);
	this->calculateRookMoves(PieceColor(white), calculateExtended);
	
	this->calculateKnightMoves(PieceColor(white), calculateExtended);
	this->calculateQueenMoves(PieceColor(white), calculateExtended);

	this->calculatePawnMoves(PieceColor(white), calculateExtended);

	this->calculateKingMoves(PieceColor(white), calculateExtended);

}

void Chessboard::calculateBlackMoves(bool calculateExtended) {
	this->blackMovesCount = 0;
	this->calculateBishopMoves(PieceColor(black), calculateExtended);
	this->calculateRookMoves(PieceColor(black), calculateExtended);
	
	this->calculateKnightMoves(PieceColor(black), calculateExtended);
	this->calculateQueenMoves(PieceColor(black), calculateExtended);
	
	this->calculatePawnMoves(PieceColor(black), calculateExtended);
	this->calculateKingMoves(PieceColor(black), calculateExtended);
}

void Chessboard::calculateKnightMoves(PieceColor color, bool calculateExtended) {

	uint64_t combinedOppositeColorBoard;
	uint64_t combinedSameColorBoard;

	const int dx[8] = { 2,  2,  1,  1, -1, -1, -2, -2 };
	const int dy[8] = { 1, -1,  2, -2,  2, -2,  1, -1 };

	int x;
	int y;

	int newX;
	int newY;

	std::pair<int, int> gridCoords;

	Move moveToAdd;

	int newIdx;

	uint64_t knightBoard;
	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		knightBoard = this->whiteKnights;


	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		knightBoard = this->blackKnights;
	}

	//iterate through the knights chessboard
	unsigned long index;

	while (knightBoard) {
		if (_BitScanForward64(&index, knightBoard)) { // Get index of the least significant 1-bit
			gridCoords = this->convertGridCoords(63 - static_cast<int>(index));
			knightBoard &= (knightBoard - 1); // Remove the lowest set bit

			x = gridCoords.first;
			y = gridCoords.second;
			//iterate through all the knights possible moves
			for (int j = 0; j < 8; j++) {
				newX = dx[j] + x;
				newY = dy[j] + y;

				if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {

					newIdx = this->convertGridCoords(std::make_pair(newX, newY));

					if (this->noBitInBoard(combinedSameColorBoard,newIdx)) {
						moveToAdd = Move(PieceType(Knight), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
						moveToAdd.eats = this->eats(moveToAdd);
						if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { continue; }
						if (color == PieceColor(white)) {
							this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
						}
						else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}
				}
			}
		}
	}
}

void Chessboard::calculatePawnMoves(PieceColor color, bool calculateExtended) {
	
	uint64_t combinedOppositeColorBoard;
	uint64_t combinedSameColorBoard;

	int dy;
	int startPawnValue;

	if (color == PieceColor(white)) { dy = -1; startPawnValue = 6; }
	else { dy = 1; startPawnValue = 1; }

	//For diagonal checks
	const int dx[2] = { 1,-1 };


	int x;
	int y;


	std::pair<int, int> gridCoords;


	Move moveToAdd;

	int newIdx;
	uint64_t pawnBoard;
	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		pawnBoard = this->whitePawns;


	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		pawnBoard = this->blackPawns;
	}

	unsigned long index;

	while (pawnBoard) {
		if (_BitScanForward64(&index, pawnBoard)) { // Get index of the least significant 1-bit
			gridCoords = this->convertGridCoords(63 - static_cast<int>(index));
			pawnBoard &= (pawnBoard - 1); // Remove the lowest set bit

			x = gridCoords.first;
			y = gridCoords.second;

			//Two squares forward
			newIdx = this->convertGridCoords(std::make_pair(x, y + 2 * dy));
			if (y == startPawnValue and this->noBitInBoard(combinedSameColorBoard |combinedOppositeColorBoard, newIdx)&&
				this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx- 8*dy)) {
				moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x, y + 2 * dy));
				if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
					if (color == PieceColor(white)) { this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd); }
					else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
				}
			}

			//One Square forward
			newIdx = this->convertGridCoords(std::make_pair(x, y + 1 * dy));
			if (this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx)) {
				moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x, y + 1 * dy));
				if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {

					//Check for promotions
					if ((color == white && y + 1 * dy == 0) || (color == black && y + 1 * dy == 7)) {
						for (int i = Queen; i <= Knight; i++) {
							moveToAdd.promotesTo = static_cast<PieceType>(i);
							if (color == PieceColor(white)) { this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd); }
							else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
						}
					}
					else {
						if (color == PieceColor(white)) { this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd); }
						else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}


					
				}
			}

			//Diagonal checks
			for (int diag : dx) {

				//Prevent going out of bounds
				if (x + diag < 0 || x + diag > 7 || y + 1 * dy < 0 || y + 1 * dy > 7) { continue; }


				newIdx = this->convertGridCoords(std::make_pair(x + diag, y + 1 * dy));
				if (this->bitInBoard( combinedOppositeColorBoard, newIdx)) {
					moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x + diag, y + 1 * dy));
					moveToAdd.eats = this->eats(moveToAdd);

					if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
						if (color == PieceColor(white)) { this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd); }
						else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}
				}


				//En passant checks
				if (this->enPassantSquare.has_value() && std::make_pair(x + diag, y + 1 * dy) == this->enPassantSquare.value()) {
					moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x + diag, y + 1 * dy));

					
					if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {

						if (color == PieceColor(white)) { this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd); }
						else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}
				}

			}


		}
		

	}
}

void Chessboard::calculateBishopMoves(PieceColor color, bool calculateExtended) {
	uint64_t combinedOppositeColorBoard;
	uint64_t combinedSameColorBoard;
	const int dx[4] = { 1,1,-1,-1 };
	const int dy[4] = { 1,-1,1,-1 };

	int x;
	int y;

	int p;

	
	int newX;
	int newY;

	std::pair<int, int> gridCoords;


	Move moveToAdd;

	int newIdx;

	uint64_t bishopBoard;
	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		bishopBoard = this->whiteBishops;



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		bishopBoard = this->blackBishops;
	}
	unsigned long index;

	while (bishopBoard) {
		if (_BitScanForward64(&index, bishopBoard)) { // Get index of the least significant 1-bit
			gridCoords = this->convertGridCoords(63 - static_cast<int>(index));
			bishopBoard &= (bishopBoard - 1); // Remove the lowest set bit

			x = gridCoords.first;
			y = gridCoords.second;
			//iterate through all the bishops directions
			for (int j = 0; j < 4; j++) {

				p = 1;
				newX = x + dx[j] * p;
				newY = y + dy[j] * p;
				newIdx = this->convertGridCoords(std::make_pair(newX, newY));

				while ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {

					newX = x + dx[j] * p;
					newY = y + dy[j] * p;
					newIdx = this->convertGridCoords(std::make_pair(newX, newY));

					//Check bounds and that it doesnt collide with same colored piece
					if (this->noBitInBoard(combinedSameColorBoard,newIdx)
						&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
						moveToAdd = Move(PieceType(Bishop), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
						moveToAdd.eats = this->eats(moveToAdd);

						if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { p += 1; 
						if (this->bitInBoard(combinedOppositeColorBoard, newIdx) || 0 > newX || newX > 7 || 0 > newY || newY > 7) {
							break;
						}
						continue; }

						if (color == PieceColor(white)) {
							this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
						}
						else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}
					if (this->bitInBoard(combinedOppositeColorBoard, newIdx) || 0 > newX || newX > 7 || 0 > newY || newY > 7) { break; }
					p++;
				}
			}
		}

	}
}

void Chessboard::calculateRookMoves(PieceColor color, bool calculateExtended) {
	uint64_t combinedOppositeColorBoard;
	uint64_t combinedSameColorBoard;

	const int dx[8] = { 1,0,-1,0 };
	const int dy[8] = { 0,1,0,-1 };
	int x;
	int y;

	int p;


	int newX;
	int newY;

	std::pair<int, int> gridCoords;


	Move moveToAdd;

	int newIdx;
	uint64_t rookBoard;

	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		rookBoard = this->whiteRooks;


	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		rookBoard = this->blackRooks;
	}
	unsigned long index;

	while (rookBoard) {
		if (_BitScanForward64(&index, rookBoard)) { // Get index of the least significant 1-bit
			gridCoords = this->convertGridCoords(63 - static_cast<int>(index));
			rookBoard &= (rookBoard - 1); // Remove the lowest set bit

			x = gridCoords.first;
			y = gridCoords.second;
			//iterate through all the rooks directions
			for (int j = 0; j < 4; j++) {

				p = 1;
				newX = x + dx[j] * p;
				newY = y + dy[j] * p;

				newIdx = this->convertGridCoords(std::make_pair(newX, newY));

				while (!(combinedSameColorBoard & (1ULL << 64 - newIdx - 1))
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
					newX = x + dx[j] * p;
					newY = y + dy[j] * p;
					newIdx = this->convertGridCoords(std::make_pair(newX, newY));

					//Check bounds and that it doesnt collide with same colored piece
					if (this->noBitInBoard(combinedSameColorBoard,newIdx)
						&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
						moveToAdd = Move(PieceType(Rook), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
						moveToAdd.eats = this->eats(moveToAdd);
						if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { p += 1; 
						if (this->bitInBoard(combinedOppositeColorBoard, newIdx) || 0 > newX || newX > 7 || 0 > newY || newY > 7) {
							break;
						}
						continue; }

						if (color == PieceColor(white)) {
							this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
						}
						else {  this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}
					if (this->bitInBoard(combinedOppositeColorBoard, newIdx) || 0 > newX || newX > 7 || 0 > newY || newY > 7) {
						break;
					}
					p += 1;
				}
			}

		}
	}
}

void Chessboard::calculateQueenMoves(PieceColor color, bool calculateExtended) {
	uint64_t combinedOppositeColorBoard;
	uint64_t combinedSameColorBoard;

	const int dx[8] = { 1,1,-1,-1,1,0,-1,0 };
	const int dy[8] = { 1,-1,1,-1,0,1,0,-1 };

	int x;
	int y;

	int p;


	int newX;
	int newY;

	std::pair<int, int> gridCoords;

	std::vector<int> indexes;

	Move moveToAdd;

	int newIdx;
	uint64_t queenBoard;
	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		queenBoard = this->whiteQueens;


	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		queenBoard = this->blackQueens;
	}

	unsigned long index;

	while (queenBoard) {
		if (_BitScanForward64(&index, queenBoard)) { // Get index of the least significant 1-bit
			gridCoords = this->convertGridCoords(63 - static_cast<int>(index));
			queenBoard &= (queenBoard - 1); // Remove the lowest set bit

			x = gridCoords.first;
			y = gridCoords.second;
			//iterate through all the bishops directions
			for (int j = 0; j < 8; j++) {

				p = 1;
				newX = x + dx[j] * p;
				newY = y + dy[j] * p;

				newIdx = this->convertGridCoords(std::make_pair(newX, newY));

				while (!(combinedSameColorBoard & (1ULL << 64 - newIdx - 1))
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
					newX = x + dx[j] * p;
					newY = y + dy[j] * p;

					newIdx = this->convertGridCoords(std::make_pair(newX, newY));

					//Check bounds and that it doesnt collide with same colored piece
					if (this->noBitInBoard(combinedSameColorBoard,newIdx)
						&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
						moveToAdd = Move(PieceType(Queen), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
						moveToAdd.eats = this->eats(moveToAdd);
						if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { p += 1; 
						if (this->bitInBoard(combinedOppositeColorBoard, newIdx) || 0 > newX || newX > 7 || 0 > newY || newY > 7) {
							break;
						}
						continue; }

						if (color == PieceColor(white)) {
							this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
						}
						else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
					}
					if (this->bitInBoard(combinedOppositeColorBoard, newIdx) || 0 > newX || newX > 7 || 0 > newY || newY > 7) { break; }
					p++;
				}
			}

		}
	}
}

void Chessboard::calculateKingMoves(PieceColor color, bool calculateExtended) {
	uint64_t combinedOppositeColorBoard;
	uint64_t combinedSameColorBoard;

	const int dx[8] = { 1,1,-1,-1,1,0,-1,0 };
	const int dy[8] = { 1,-1,1,-1,0,1,0,-1 };

	int x;
	int y;


	int newX;
	int newY;

	std::pair<int, int> gridCoords;

	uint64_t kingBoard;
	Move moveToAdd;

	int newIdx;

	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		kingBoard = this->whiteKing;



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));
		kingBoard = this->blackKing;

	}
	unsigned long index;
	while (kingBoard) {
		if (_BitScanForward64(&index, kingBoard)) { // Get index of the least significant 1-bit
			gridCoords = this->convertGridCoords(63 - static_cast<int>(index));
			kingBoard &= (kingBoard - 1); // Remove the lowest set bit

			x = gridCoords.first;
			y = gridCoords.second;
			//iterate through all the kings directions
			for (int j = 0; j < 8; j++) {
				newX = x + dx[j];
				newY = y + dy[j];

				newIdx = this->convertGridCoords(std::make_pair(newX, newY));

				//Check bounds and that it doesnt collide with same colored piece
				if (this->noBitInBoard(combinedSameColorBoard,newIdx)
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
					moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
					moveToAdd.eats = this->eats(moveToAdd);
					if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { continue; }
					if (color == PieceColor(white)) {
						
						this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
					}
					else { this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd); }
				}
			}


			if (!calculateExtended || !this->isInCheck(color)) {
				if (color == white) {
					newIdx = this->convertGridCoords(std::make_pair(x + 2, y));
					if (x == 4 && y == 7 && this->whiteKingCastling &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx) &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx-1)) {
						Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x + 2, y));
						if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
							if (!this->checkInPath(y, x, x + 2, King, white)) {
								this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
							}
						}
					}
					newIdx = this->convertGridCoords(std::make_pair(x - 3, y));
					if (x == 4 && y == 7 && this->whiteQueenCastling &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx) &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx + 1))
					{
						Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x - 2, y));

						if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
							if (!this->checkInPath(y, x, x -2, King, white)) {
								this->whiteMoves[this->whiteMovesCount++] = std::move(moveToAdd);
							}
						}
					}
				}
				else {
					newIdx = this->convertGridCoords(std::make_pair(x + 2, y));
					if (x == 4 && y == 0 && this->blackKingCastling &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx) &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx - 1)) {
						Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x + 2, y));

						if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
							if (!this->checkInPath(y, x, x + 2, King, black)) {
								this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd);
							}
						}
					}
					newIdx = this->convertGridCoords(std::make_pair(x - 3, y));
					if (x == 4 && y == 0 && this->blackQueenCastling &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx) &&
						this->noBitInBoard(combinedSameColorBoard | combinedOppositeColorBoard, newIdx + 1)) {   
						Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x - 2, y));

						if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
							if (!this->checkInPath(y, x, x - 2, King, black)) {
								this->blackMoves[this->blackMovesCount++] = std::move(moveToAdd);
							}
						}
					}
				}
			}
		}
	}
}

bool Chessboard::checkInPath(int y, int startX, int endX, PieceType type, PieceColor color){
	if (startX <= endX) {
		for (int i = startX; i < endX; i++) {
			Move moveToCheck(type, color, std::make_pair(startX, y), std::make_pair(i, y));
			if (this->wouldMoveCauseCheck(moveToCheck)) {
				return true;
			}

		}
	}
	else {
		for (int i = startX; i < endX; i--) {
			Move moveToCheck(type, color, std::make_pair(startX, y), std::make_pair(i, y));
			if (this->wouldMoveCauseCheck(moveToCheck)) {
				return true;
			}

		}
	}
	return false;
}


bool Chessboard::isInCheck(PieceColor color) {
	if (color == white) {
		this->calculateBlackMoves(false);
		for (int i = 0; i < this->blackMovesCount; i++) {
			if (this->blackMoves[i].to == this->getKingPosition(white)) {
				return true;
			}
		}
	}
	else {
		this->calculateWhiteMoves(false);
		for (int i = 0; i < this->whiteMovesCount; i++) {
			if (this->whiteMoves[i].to == this->getKingPosition(black)) {
				return true;
			}
		}
	}
	return false;
}

uint64_t* Chessboard::getBoardOfType(PieceType type, PieceColor color) {
	switch (type) {
	case Bishop: return color == PieceColor(white) ? &this->whiteBishops : &this->blackBishops; break;
	case Queen: return color == PieceColor(white) ? &this->whiteQueens : &this->blackQueens; break;
	case Pawn: return color == PieceColor(white) ? &this->whitePawns : &this->blackPawns; break;
	case Rook: return color == PieceColor(white) ? &this->whiteRooks : &this->blackRooks; break;
	case Knight: return color == PieceColor(white) ? &this->whiteKnights : &this->blackKnights; break;
	case King: return color == PieceColor(white) ? &this->whiteKing : &this->blackKing; break;
	}
}

void Chessboard::executeMove(Move& move) {

	this->history.push_back({
		this->enPassantSquare,
		this->whiteMoves,
		this->blackMoves,
		this->whiteMovesCount,
		this->blackMovesCount,
		this->whiteKingCastling,
		this->whiteQueenCastling,
		this->blackKingCastling,
		this->blackQueenCastling,
		this->halfMoves
		});


	PieceColor color = move.pieceColor;
	std::pair<int, int> from = move.from;
	std::pair<int, int> to = move.to;
	PieceType type = move.pieceType;
	
	

	uint64_t* currBoard = this->getBoardOfType(type, color);


	//delete current position
	*currBoard &= ~(1ULL << (63 - this->convertGridCoords(from)));


	//add new Position 
	*currBoard |= (1ULL << (63 - this->convertGridCoords(to)));
	
	
	
	if (type == King && std::abs(from.first - to.first) == 2) {
		Move rookMoveCastling;
		if (to == std::make_pair(2, 0) && this->blackQueenCastling) {
			rookMoveCastling = Move(Rook, black, std::make_pair(0, 0), std::make_pair(3, 0));
			this->executeMove(rookMoveCastling);
			this->toMove = this->toMove == white ? black : white;
			if (this->toMove == PieceColor(black)) {
				this->fullMoves -= 1;
				
			}
		}
		else if (to == std::make_pair(6, 0) && this->blackKingCastling) {
			rookMoveCastling = Move(Rook, black, std::make_pair(7, 0), std::make_pair(5, 0));
			this->executeMove(rookMoveCastling);
			this->toMove = this->toMove == white ? black : white;
			
			if (this->toMove == PieceColor(black)) {
				this->fullMoves -= 1;
				
			}
		}
		else if (to == std::make_pair(2, 7) && this->whiteQueenCastling) {
			rookMoveCastling = Move(Rook, white, std::make_pair(0, 7), std::make_pair(3, 7));
			this->executeMove(rookMoveCastling);
			this->toMove = this->toMove == white ? black : white;
		}
		else if (to == std::make_pair(6, 7) && this->whiteKingCastling) {
			rookMoveCastling = Move(Rook, white, std::make_pair(7, 7), std::make_pair(5, 7));
			this->executeMove(rookMoveCastling);
			this->toMove = this->toMove == white ? black : white;
		}
		this->halfMoves -= 1;
	}
	//If kings are moved, disable castling
	if (type == King) {
		if (color == black) { this->blackKingCastling = false; this->blackQueenCastling = false; }
		else { this->whiteKingCastling = false; this->whiteQueenCastling = false; }
	}
	//If any other piece arrives at the edge squares, disable the castling at that edge
	if (to == std::make_pair(0, 0) || from == std::make_pair(0, 0)) { this->blackQueenCastling = false; }
	if (to == std::make_pair(7, 0) || from == std::make_pair(7, 0)) { this->blackKingCastling = false; }
	if (to == std::make_pair(0, 7) || from == std::make_pair(0, 7)) { this->whiteQueenCastling = false; }
	if (to == std::make_pair(7, 7) || from == std::make_pair(7, 7)) { this->whiteKingCastling = false; }

	//Check for en passant and remove according pawn
	if (type == Pawn && this->enPassantSquare.has_value() && to == this->enPassantSquare.value()) {
		if (color == white) {
			std::pair<int, int> squareToDelete = std::make_pair(to.first, to.second + 1);
			this->blackPawns &= ~(1ULL << (63 - this->convertGridCoords(squareToDelete)));
		}
		else {
			std::pair<int, int> squareToDelete = std::make_pair(to.first, to.second - 1);
			this->whitePawns &= ~(1ULL << (63 - this->convertGridCoords(squareToDelete)));
		}
	}

	//Enable and disable en passant square
	if (type == Pawn && std::abs(from.second - to.second) == 2) {
		if (color == black) {
			this->enPassantSquare = std::make_pair(to.first, to.second - 1);
		}
		else {
			this->enPassantSquare = std::make_pair(to.first, to.second + 1);
		}
	}
	else {
		this->enPassantSquare = std::nullopt;
	}


	bool captureMade = false;
	//delete position in opposite colored boards in case of capture
	if (color == PieceColor(white)) {
		//Check for capture
		if ((((uint64_t)this->getCombinedBoard(PieceColor(black)) & (1ULL << 63 - this->convertGridCoords(to))) != 0)) {
			captureMade = true;
		}


		this->blackPawns &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->blackKnights &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->blackBishops &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->blackQueens &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->blackRooks &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->blackKing &= ~(1ULL << (63 - this->convertGridCoords(to)));


	}
	else {
		if ((((uint64_t)this->getCombinedBoard(PieceColor(white)) & (1ULL << 63 - this->convertGridCoords(to))) != 0)) {
			captureMade = true;
		}
		this->whitePawns &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteKnights &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteBishops &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteQueens &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteRooks &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteKing &= ~(1ULL << (63 - this->convertGridCoords(to)));

		
	}

	if (move.pieceType == PieceType(Pawn) || move.eats.has_value()) {
		this->halfMoves = 0;
	}
	else {
		this->halfMoves += 1; // Otherwise, increment half-move clock
	}

	// Increment fullMoves only when White moves
	if (this->toMove == PieceColor(black)) {
		this->fullMoves += 1;
	}

	//remove Move from set
	if (color == PieceColor(white)) {
		this->whiteMovesCount = 0;
	}
	else {
		this->blackMovesCount = 0;
	}

	//change color of toMove
	this->toMove = this->toMove == PieceColor(black) ? PieceColor(white) : PieceColor(black);

}

std::vector<int> Chessboard::getBitIndexes(uint64_t n) {
	std::vector<int> indexes;
	unsigned long index; // MSVC requires this type for _BitScanForward64
	while (n) {
		if (_BitScanForward64(&index, n)) { // Get index of the least significant 1-bit
			indexes.push_back(63 - static_cast<int>(index));
			n &= (n - 1); // Remove the lowest set bit
		}
	}


	return indexes;
}

bool Chessboard::wouldMoveCauseCheck(Move& move) {
	// Execute the move on the cloned board
	this->executeMove(move);
	// Get the position of the king after the move
	std::pair<int, int> kingPos;
	std::array<Move,MAX_MOVES> moves;
	int moveCount;
	if (move.pieceColor == white) {
		kingPos = this->getKingPosition(white);
		this->calculateBlackMoves(false);
		moves = std::move(this->blackMoves);
		moveCount = this->blackMovesCount;
	}
	else {
		kingPos = this->getKingPosition(black);
		this->calculateWhiteMoves(false);
		moves = std::move(this->whiteMoves);
		moveCount = this->whiteMovesCount;
	}
	Move opponentMove;
	for (size_t i = 0; i < moveCount; i++) {
		opponentMove = std::move(moves[i]);
		if (opponentMove.to == kingPos) {
			this->undoMove(move);
			return true;
		}
	}

	this->undoMove(move);
	return false;
}

uint64_t Chessboard::bitAt(int idx) {
	return (uint64_t)(1ULL << (63 - idx));
}

bool Chessboard::bitInBoard(uint64_t board, int idx) {
	return ((board & this->bitAt(idx)) != 0);
}

bool Chessboard::noBitInBoard(uint64_t board, int idx) {
	return ((board & this->bitAt(idx)) == 0);
}

std::pair<int, int> Chessboard::getKingPosition(PieceColor color) {
	std::pair<int, int> ans;
	if (color == white) {
		ans = this->convertGridCoords(this->getBitIndexes(this->whiteKing)[0]);
	}
	else {
		ans = this->convertGridCoords(this->getBitIndexes(this->blackKing)[0]);
	}

	
	return ans;
}

std::string Chessboard::getFEN() {
	char board[8][8] = {};
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			board[r][c] = ' '; // Empty space initially
		}
	}

	auto setPiece = [&](uint64_t bitboard, char piece) {
		for (int i = 0; i < 64; i++) {
			if (bitboard & (1ULL << i)) {
				int row = 7 -(i / 8);
				int col = 7-(i % 8);
				board[row][col] = piece;
			}
		}
		};

	setPiece(this->blackPawns, 'p');
	setPiece(this->blackRooks, 'r');
	setPiece(this->blackBishops, 'b');
	setPiece(this->blackKnights, 'n');
	setPiece(this->blackQueens, 'q');
	setPiece(this->blackKing, 'k');

	setPiece(this->whitePawns, 'P');
	setPiece(this->whiteRooks, 'R');
	setPiece(this->whiteBishops, 'B');
	setPiece(this->whiteKnights, 'N');
	setPiece(this->whiteQueens, 'Q');
	setPiece(this->whiteKing, 'K');

	std::string fen = "";
	for (int r = 0; r < 8; r++) {
		int emptyCount = 0;
		for (int c = 0; c < 8; c++) {
			if (board[r][c] == ' ') {
				emptyCount++;
			}
			else {
				if (emptyCount > 0) {
					fen += std::to_string(emptyCount);
					emptyCount = 0;
				}
				fen += board[r][c];
			}
		}
		if (emptyCount > 0) {
			fen += std::to_string(emptyCount);
		}
		if (r < 7) {
			fen += '/';
		}
	}

	fen += " ";
	fen += (toMove == PieceColor::white) ? 'w' : 'b';
	fen += " ";

	std::string castling = "";
	castling += this->whiteKingCastling ? "K" : "-";
	castling += this->whiteQueenCastling? "Q" : "-";
	castling += this->blackKingCastling ? "k" : "-";
	castling += this->blackQueenCastling ? "q" : "-";
	fen += castling + " ";

	if (this->enPassantSquare.has_value()) {
		int col = (*this->enPassantSquare).first;
		int row = 8 - (*this->enPassantSquare).second;
		fen += (char)('a' + col);
		fen += std::to_string(row);
	}
	else {
		std::cout << "no en passant" << std::endl;
		fen += "-";
	}

	fen += " " + std::to_string(this->halfMoves) + " " + std::to_string(this->fullMoves);
	return fen;
}

std::map<PieceType, int> Chessboard::getMaterial(PieceColor color) {
	std::map<PieceType, int> answer;
	if (color == white) {
		answer[Pawn] = this->getBitIndexes(this->whitePawns).size();
		answer[Knight] = this->getBitIndexes(this->whiteKnights).size();
		answer[Rook] = this->getBitIndexes(this->whiteRooks).size();
		answer[Bishop] = this->getBitIndexes(this->whiteBishops).size();
		answer[Queen] = this->getBitIndexes(this->whiteQueens).size();
		answer[King] = this->getBitIndexes(this->whiteKing).size();
	}
	else {
		answer[Pawn] = this->getBitIndexes(this->blackPawns).size();
		answer[Knight] = this->getBitIndexes(this->blackKnights).size();
		answer[Rook] = this->getBitIndexes(this->blackRooks).size();
		answer[Bishop] = this->getBitIndexes(this->blackBishops).size();
		answer[Queen] = this->getBitIndexes(this->blackQueens).size();
		answer[King] = this->getBitIndexes(this->blackKing).size();
	}

	return answer;
}

int Chessboard::simpleEvaluation() {
#ifdef _MSC_VER
#define POPCOUNT64(x) __popcnt64(x)
#else
#define POPCOUNT64(x) __builtin_popcountll(x)
#endif

	return (POPCOUNT64(whitePawns) * 100 +
		POPCOUNT64(whiteKnights) * 300 +
		POPCOUNT64(whiteBishops) * 300 +
		POPCOUNT64(whiteRooks) * 500 +
		POPCOUNT64(whiteQueens) * 900 -
		(POPCOUNT64(blackPawns) * 100 +
			POPCOUNT64(blackKnights) * 300 +
			POPCOUNT64(blackBishops) * 300 +
			POPCOUNT64(blackRooks) * 500 +
			POPCOUNT64(blackQueens) * 900) )*(this->toMove == white ? 1 : -1);
}

Move Chessboard::calculateBestMove() {
	std::array<Move,MAX_MOVES> movesToCheck;
	int bestScore = INT_MIN;
	Move bestMove;
	int moveCount;
	// Select moves based on the current player
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		movesToCheck = std::move(this->whiteMoves);
		moveCount = this->whiteMovesCount;
		bestScore = INT_MIN;

		  // Maximize for white
	}
	else {
		this->calculateBlackMoves();
		movesToCheck = std::move(this->blackMoves);
		bestScore = INT_MAX;// Minimize for black
		moveCount = this->blackMovesCount;
	}



	int currScore;
	Move move;
	// Iterate through all valid moves
	for (size_t i = 0; i < moveCount; i++) {
		move = std::move(movesToCheck[i]);
		this->executeMove(move);
		if (this->toMove == white) {
			this->calculateWhiteMoves();
		}
		else {
			this->calculateBlackMoves();
		}
		currScore = (this->toMove == white ? 1 : -1)*this->negaMax(DEPTH-1);

		// Print the current move and score
		std::cout << "Checking root: " << move << " : " << currScore << std::endl;

		// Update best move based on the current player's strategy (maximize or minimize)
		if ((this->toMove == black&& currScore >= bestScore) ||
			(this->toMove == white && currScore <= bestScore)) {
			bestMove = std::move(move);
			bestScore = currScore;
			
		}
	
		this->undoMove(move);
	}
	// Output the best move
	std::cout << "Chosen move: " << bestMove << " : " << bestScore << std::endl;
	return bestMove;
}

int Chessboard::negaMax(int depth) {

	if (depth == 0) {
		return this->simpleEvaluation();  // Base case: return evaluation at depth 0
	}


	std::array<Move,MAX_MOVES> movesToCheck;
	int moveCount;
	int bestScore = INT_MIN;

	// Select moves based on the current player
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		moveCount = this->whiteMovesCount;
		movesToCheck = std::move(this->whiteMoves);
	}
	else {
		this->calculateBlackMoves();
		moveCount = this->blackMovesCount;
		movesToCheck = std::move(this->blackMoves);
	}

	int currScore;
	Chessboard tempBoard;

	// If no moves are possible, it's either a loss or a draw
	if (moveCount == 0) {
		return MIN_SCORE;  // Loss for the current player
	}
	Move move;
	// Iterate through all possible moves
	for (size_t i = 0; i < moveCount; i++) {
		this->executeMove(movesToCheck[i]);
		if (this->toMove == white) {
			this->calculateWhiteMoves();
		}
		else {
			this->calculateBlackMoves();
		}
		currScore = -this->negaMax(depth - 1);  // Negate score for alternating turns
		// Update best score based on the current player’s strategy
		if (currScore > bestScore) {
			bestScore = currScore;
		}
		this->undoMove(movesToCheck[i]);
		
	}

	return bestScore;
}

int Chessboard::negaMaxAlphaPruning(int depth, int alpha, int beta) {
	if (depth == 0) {
		return this->simpleEvaluation();  // Base case: return evaluation at depth 0
	}

	std::array<Move,MAX_MOVES> movesToCheck;
	int bestScore = INT_MIN;
	int moveCount;
	// Select moves based on the current player
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		moveCount = this->whiteMovesCount;
		movesToCheck = std::move(this->whiteMoves);
	}
	else {
		this->calculateBlackMoves();
		moveCount = this->blackMovesCount;
		movesToCheck = std::move(this->blackMoves);
	}

	int currScore;
	Chessboard tempBoard = *this;		

	// If no moves are possible, it's either a loss or a draw
	if (movesToCheck.empty()) {
		return MIN_SCORE;  // Loss for the current player
	}
	Move move;
	// Iterate through all possible moves
	for (size_t i = 0; i < moveCount; i++) {
		move = std::move(movesToCheck[i]);
		// Apply the move to a temporary board and evaluate it
		
		tempBoard.executeMove(move);
		if (tempBoard.toMove == white) {
			tempBoard.calculateWhiteMoves();
		}
		else {
			tempBoard.calculateBlackMoves();
		}


		currScore = -tempBoard.negaMaxAlphaPruning(depth - 1,-beta,-alpha);  // Negate score for alternating turns

		// Update best score based on the current player’s strategy
		if (currScore > bestScore) {
			bestScore = currScore;
			if (currScore > alpha) {
				alpha = currScore;
			}

		}
		if (currScore > beta) {
			return bestScore;
		}
		tempBoard.undoMove(move);
	}

	return bestScore;
}

std::optional<Piece> Chessboard::eats(Move& mv) {
	int endPosition = this->convertGridCoords(mv.to);

	uint64_t combinedOppositeBoard = this->getCombinedBoard(mv.pieceColor == white ? black : white);

	if ((combinedOppositeBoard & 1ULL << 63 - endPosition) == 0) {
		return std::nullopt;
	}

	Piece pieceEaten = Piece();
	pieceEaten.color = mv.pieceColor == white ? black : white;
	pieceEaten.pos = mv.to;	
	if (mv.pieceColor == white) {
		if ((this->blackPawns & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Pawn;
		}
		else if ((this->blackKnights & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Knight;
		}
		else if ((this->blackBishops & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Bishop;
		}
		else if ((this->blackRooks & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Rook;
		}
		else if ((this->blackQueens & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Queen;
		}
		else if ((this->blackKing & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = King;
		}
	}
	else {
		if ((this->whitePawns & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Pawn;
		}
		else if ((this->whiteKnights & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Knight;
		}
		else if ((this->whiteBishops & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Bishop;
		}
		else if ((this->whiteRooks & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Rook;
		}
		else if ((this->whiteQueens & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = Queen;
		}
		else if ((this->whiteKing & 1ULL << 63 - endPosition) != 0) {
			pieceEaten.type = King;
		}
	}

	return pieceEaten;
}

void Chessboard::undoMove(Move& move) {
	if (!history.empty()) {
		GameState last = history.back();
		history.pop_back();

		this->enPassantSquare = last.enPassantTarget;
		this->whiteMoves = last.whiteMoves;
		this->blackMoves = last.blackMoves;
		this->whiteMovesCount = last.whiteMovesCount;
		this->blackMovesCount = last.blackMovesCount;
		this->whiteKingCastling = last.whiteKingCastling;
		this->whiteQueenCastling = last.whiteQueenCastling;
		this->blackKingCastling = last.blackKingCastling;
		this->blackQueenCastling = last.blackQueenCastling;
		this->halfMoves = last.halfmoveClock;
	}




	//handle normal moves
	uint64_t* boardOfType = this->getBoardOfType(move.pieceType, move.pieceColor);

	//Delete its current position and add old position

	//delete "new" position
	*boardOfType &= ~(1ULL << 63 - this->convertGridCoords(move.to));

	//add old position
	*boardOfType |= (1ULL << 63 - this->convertGridCoords(move.from));

	//Implies promotion
	if (move.promotesTo.has_value()) {
		uint64_t*  boardOfTypePromotion = this->getBoardOfType(move.promotesTo.value(), move.pieceColor);

		//Delete position from board
		*boardOfTypePromotion &= ~(1ULL << 63 - this->convertGridCoords(move.to));
	}

	//Check for en passant: has to be a pawn, has to have moved diagonally and cant have eaten anything
	if (move.pieceType == Pawn && abs(move.to.first - move.from.first) == 1 && !move.eats.has_value()) {
		//TODO
		uint64_t* boardOfOppositePawn = this->getBoardOfType(Pawn, move.pieceType == white ? black : white);
		std::pair<int, int> pawnCoords = move.to;
		
		if (move.pieceColor == white) {
			this->blackPawns|= (1ULL << 63 - this->convertGridCoords(std::make_pair(move.to.first , move.to.second +1)));
		}
		else {
			this->whitePawns |= (1ULL << 63 - this->convertGridCoords(std::make_pair(move.to.first, move.to.second - 1)));
		}



	}

	//Check for castling: King moved horizontally more than one square
	if (move.pieceType == King && abs(move.to.first - move.from.first) > 1) {
		//TODO

		Move rookCastlingMove;
		if (move.pieceColor == white)
		{
			//Queen side castling
			if (move.from.first > move.to.first) {
				rookCastlingMove = Move(Rook, white, std::make_pair(0, 7), std::make_pair(3, 7));
			}
			else {
				rookCastlingMove = Move(Rook, white, std::make_pair(7, 7), std::make_pair(5, 7));
			}
		}
		else
		{
			//Queen side castling
			if (move.from.first > move.to.first) {
				rookCastlingMove = Move(Rook, black, std::make_pair(0, 0), std::make_pair(3, 0));
			}
			else {
				rookCastlingMove = Move(Rook, black, std::make_pair(7, 0), std::make_pair(5, 0));
			}
			if (this->toMove == PieceColor(black)) {
				this->fullMoves += 1;

			}
		}
		this->undoMove(rookCastlingMove);
		this->halfMoves += 1;
		return;
	}

	//Restore old piece
	if (move.eats.has_value()) {
		uint64_t* boardOfEatenPiece = this->getBoardOfType(move.eats.value().type, move.eats.value().color);
		*boardOfEatenPiece |= (1ULL << 63 - this->convertGridCoords(move.to));
	}

	if (move.pieceType == PieceType(Pawn) || move.eats.has_value()) {
		this->halfMoves = 0;
	}
	else {
		this->halfMoves -= 1;
	}

	// Only decrement fullMoves if it was Black’s turn
	if (this->toMove == PieceColor(white)) {
		this->fullMoves -= 1;
	}

	this->toMove = this->toMove == white ? black : white;

	
}

int Chessboard::countMoves(int depth) {
	if (depth == 0) {
		return 1;
	}
	int ans = 0;
	std::vector<std::future<int>> futures;
	this->whiteMovesCount = 0;
	this->blackMovesCount = 0;


	std::array <Move, MAX_MOVES> movesToCheck;
	int moveCount;
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		std::memcpy(movesToCheck.data(), this->whiteMoves.data(), this->whiteMovesCount * sizeof(Move));
		moveCount = this->whiteMovesCount;
	}
	else {
		this->calculateBlackMoves();
		std::memcpy(movesToCheck.data(), this->blackMoves.data(), this->blackMovesCount * sizeof(Move));
		moveCount = this->blackMovesCount;
	}
	if (moveCount == 0) { return 1; }
	std::shared_ptr<Chessboard> boardPointer;
	for (size_t i = 0; i < moveCount; i++)
	{	

		this->executeMove(movesToCheck[i]);
		if (depth > 3 && futures.size() < maxThreads) {
			boardPointer = std::make_shared<Chessboard>(*this);
			futures.push_back(std::async(std::launch::async, [boardPointer, depth]{
				
				return boardPointer->countMoves(depth - 1);
				}));
		}
		else {
			ans += this->countMoves(depth - 1);
		}

		this->undoMove(movesToCheck[i]);
	}

	for (auto& future : futures) {
		ans += future.get();
	}
	
	return ans;
	
}

Move Chessboard::stringToMove(std::string move) {
	auto algebraicToCoord = [](const std::string& sq) -> std::pair<int, int> {
		int col = sq[0] - 97;

		int row = '8' - sq[1];
		return { col,row};
		};
	std::pair<int, int> from = algebraicToCoord(move.substr(0, 2));
	std::pair<int, int> to = algebraicToCoord(move.substr(2, 2));


	Move moveToReturn;
	moveToReturn.from = from;
	moveToReturn.to = to;

	std::pair<PieceType, PieceColor> info = this->getTypeOfPos(from);
	moveToReturn.pieceColor = info.second;
	moveToReturn.pieceType = info.first;
	moveToReturn.eats = this->eats(moveToReturn);

	return moveToReturn;
}

bool hasBitSet(uint64_t value, int n) {
	return (value & (1ULL << (63 - n))) != 0;
}

std::pair<PieceType, PieceColor> Chessboard::getTypeOfPos(std::pair<int, int> pos) {
	uint64_t convertedPos = this->convertGridCoords(pos);

	if (hasBitSet(this->whitePawns, convertedPos)) {
		return std::make_pair(Pawn, white);
	}
	if (hasBitSet(this->whiteKnights, convertedPos)) {
		return std::make_pair(Knight, white);
	}
	if (hasBitSet(this->whiteBishops, convertedPos)) {
		return std::make_pair(Bishop, white);
	}
	if (hasBitSet(this->whiteRooks, convertedPos)) {
		return std::make_pair(Rook, white);
	}
	if (hasBitSet(this->whiteQueens, convertedPos)) {
		return std::make_pair(Queen, white);
	}
	if (hasBitSet(this->whiteKing, convertedPos)) {
		return std::make_pair(King, white);
	}
	if (hasBitSet(this->blackPawns, convertedPos)) {
		return std::make_pair(Pawn, black);
	}
	if (hasBitSet(this->blackKnights, convertedPos)) {
		return std::make_pair(Knight, black);
	}
	if (hasBitSet(this->blackBishops, convertedPos)) {
		return std::make_pair(Bishop, black);
	}
	if (hasBitSet(this->blackRooks, convertedPos)) {
		return std::make_pair(Rook, black);
	}
	if (hasBitSet(this->blackQueens, convertedPos)) {
		return std::make_pair(Queen, black);
	}
	if (hasBitSet(this->blackKing, convertedPos)) {
		return std::make_pair(King, black);
	}

}

std::vector<Move> Chessboard::getStockfishMoves() {
	std::vector<Move> moves;

	auto isMove = [](std::string input) {
		return input.at(0)>= 'a' && input.at(0) <= 'h'
			&& isdigit(input.at(1))
			&& input.at(2) >= 'a' && input.at(2) <= 'h' &&
			isdigit(input.at(3));
		};


	

	
	
	return moves;
}
