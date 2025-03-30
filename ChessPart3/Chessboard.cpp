#include "Chessboard.h"
#include "pieceColor.h"
#include <bitset>
#include <cmath>
#include <intrin.h>
#include <map>
#include <cstdint>
#include <future>
#include <array>
const int height = 8;
const int width = 8;
const int kingWeight = INT_MAX;
const int queenWeight = 1000;
const int rookWeight = 525;
const int bishopWeight = 350;
const int knightWeight = 350;
const int pawnWeight = 100;
const int DEPTH = 4;
const int MIN_SCORE = -99999;
const int MAX_SCORE = 99999;
static int maxThreads = std::thread::hardware_concurrency();


Chessboard::Chessboard() {}

Chessboard::Chessboard(std::string FEN) {
	this->setFEN(FEN);

	this->oldEnPassantSquare = std::nullopt;
}

void Chessboard::setFEN(std::string FEN) {
	this->FEN = FEN;

	int n = FEN.size();
	int currentX = 0, currentY = 0;

	for (int i = 0; i < n; i++) {
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
	int i = FEN.find(' ') + 1;  // Find first space
	if (i < n) { this->toMove = (FEN[i] == 'w') ? PieceColor(white) : PieceColor(black); i++; }

	if (i < n) {
		this->whiteKingCastling = FEN.find('K', i++) != std::string::npos;
		this->whiteQueenCastling = FEN.find('Q', i++) != std::string::npos;
		this->blackKingCastling = FEN.find('k', i++) != std::string::npos;
		this->blackQueenCastling = FEN.find('q', i++) != std::string::npos;
	}
	i+= 2;
	if (i < n && FEN[i] != '-') {
		if (i + 1 < n) { // Ensure we don’t access out-of-bounds
			this->enPassantSquare = std::make_pair(FEN[i] - 'a', FEN[i + 1] - '1');
			std::cout << FEN[i] << FEN[i + 1] << std::endl;
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
	this->whiteMoves.clear();

	this->calculateBishopMoves(PieceColor(white), calculateExtended);
	this->calculateRookMoves(PieceColor(white), calculateExtended);
	
	this->calculateKnightMoves(PieceColor(white), calculateExtended);
	this->calculateQueenMoves(PieceColor(white), calculateExtended);

	this->calculatePawnMoves(PieceColor(white), calculateExtended);

	this->calculateKingMoves(PieceColor(white), calculateExtended);

}

void Chessboard::calculateBlackMoves(bool calculateExtended) {
	this->blackMoves.clear();
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

	std::vector<int> indexes;

	Move moveToAdd;

	int newIdx;
	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		indexes = this->getBitIndexes(this->whiteKnights);



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		indexes = this->getBitIndexes(this->blackKnights);
	}

	//iterate through the knights chessboard
	for (int i : indexes) {
		gridCoords = this->convertGridCoords(i);

		x = gridCoords.first;
		y = gridCoords.second;
		//iterate through all the knights possible moves
		for (int j = 0; j < 8; j++) {
			newX = dx[j] + x;
			newY = dy[j] + y;

			if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8) {

				newIdx = this->convertGridCoords(std::make_pair(newX, newY));

				if ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0) {
					moveToAdd = Move(PieceType(Knight), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
					moveToAdd.eats = this->eats(moveToAdd);
					if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { continue; }
					if (color == PieceColor(white)) {
						this->whiteMoves.emplace_back(moveToAdd);
					}
					else { this->blackMoves.emplace_back(moveToAdd); }
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

	std::vector<Move> movesToAddSet;

	int x;
	int y;


	std::pair<int, int> gridCoords;

	std::vector<int> indexes;

	Move moveToAdd;

	int newIdx;
	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		indexes = this->getBitIndexes(this->whitePawns);



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		indexes = this->getBitIndexes(this->blackPawns);
	}

	//iterate through the pawns chessboard
	for (int i : indexes) {


		gridCoords = this->convertGridCoords(i);

		x = gridCoords.first;
		y = gridCoords.second;

		//Two squares forward
		newIdx = this->convertGridCoords(std::make_pair(x, y + 2 * dy));
		if (y == startPawnValue and (uint64_t)((combinedSameColorBoard | combinedOppositeColorBoard) &(1ULL << 64 - newIdx - 1)) == 0 &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx  + 8 * dy)) == 0) {
			moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x, y + 2 * dy));
			moveToAdd.eats = this->eats(moveToAdd);
			if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
				movesToAddSet.emplace_back(moveToAdd);
			}
		}

		//One Square forward
		newIdx = this->convertGridCoords(std::make_pair(x, y + 1 * dy));
		if ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0 && (uint64_t)(combinedOppositeColorBoard & (1ULL << 64 - newIdx - 1)) == 0) {
			moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x, y + 1 * dy));
			moveToAdd.eats = this->eats(moveToAdd);
			if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
				movesToAddSet.emplace_back(moveToAdd);
			}
		}

		//Diagonal checks
		for (int diag : dx) {

			//Prevent going out of bounds
			if (x + diag < 0 || x + diag > 7 || y + 1 * dy < 0 || y + 1 * dy > 7) { continue; }


			newIdx = this->convertGridCoords(std::make_pair(x + diag, y + 1 * dy));
			if ((uint64_t)(combinedOppositeColorBoard & (1ULL << 64 - newIdx - 1)) != 0) {
				moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x + diag, y + 1 * dy));
				moveToAdd.eats = this->eats(moveToAdd);
				
				if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
					movesToAddSet.emplace_back(moveToAdd);
				}
			}
			

			//En passant checks
			if (this->enPassantSquare.has_value() && std::make_pair(x + diag, y + 1 * dy) == this->enPassantSquare.value()) {
				moveToAdd = Move(PieceType(Pawn), PieceColor(color), std::make_pair(x, y), std::make_pair(x + diag, y + 1 * dy));

				moveToAdd.eats = this->eats(moveToAdd);
				if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {

					movesToAddSet.emplace_back(moveToAdd);
				}
			}

		}


	}
	if (color == PieceColor(white)) {
		for (const Move& move : movesToAddSet) {
			this->whiteMoves.emplace_back(move);
		}
	}
	else {
		for (const Move& move : movesToAddSet) {
			this->blackMoves.emplace_back(move);
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

	std::vector<int> indexes;

	Move moveToAdd;

	int newIdx;

	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		indexes = this->getBitIndexes(this->whiteBishops);



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		indexes = this->getBitIndexes(this->blackBishops);
	}
	for (int i : indexes) {


		gridCoords = this->convertGridCoords(i);

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
				if ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
					moveToAdd = Move(PieceType(Bishop), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
					moveToAdd.eats = this->eats(moveToAdd);
					
					if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { p += 1; continue; }

					if (color == PieceColor(white)) { this->whiteMoves.emplace_back(moveToAdd); }
					else { this->blackMoves.emplace_back(moveToAdd); }
				}
				if ((uint64_t)(combinedOppositeColorBoard & (1ULL << 64 - newIdx - 1)) != 0 || 0 >= newX || newX > 8 || 0 >= newY || newY > 8) { break; }
				p++;
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

	std::vector<int> indexes;

	Move moveToAdd;

	int newIdx;

	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		indexes = this->getBitIndexes(this->whiteRooks);



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		indexes = this->getBitIndexes(this->blackRooks);
	}

	for (int i : indexes) {

		gridCoords = this->convertGridCoords(i);

		x = gridCoords.first;
		y = gridCoords.second;
		//iterate through all the rooks directions
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
				if ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
					moveToAdd = Move(PieceType(Rook), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
					moveToAdd.eats = this->eats(moveToAdd);
					if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { p += 1; continue; }

					if (color == PieceColor(white)) { this->whiteMoves.emplace_back(moveToAdd); }
					else { this->blackMoves.emplace_back(moveToAdd); }
				}
				if ((uint64_t)(combinedOppositeColorBoard & (1ULL << 64 - newIdx - 1)) != 0 || 0 > newX || newX >= 8 || 0 > newY || newY >= 8) { break; }				p+= 1;
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

	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		indexes = this->getBitIndexes(this->whiteQueens);



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		indexes = this->getBitIndexes(this->blackQueens);
	}

	for (int i : indexes) {
		gridCoords = this->convertGridCoords(i);

		x = gridCoords.first;
		y = gridCoords.second;
		//iterate through all the bishops directions
		for (int j = 0; j < 8; j++) {

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
				if ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0
					&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
					moveToAdd = Move(PieceType(Queen), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
					moveToAdd.eats = this->eats(moveToAdd);
					if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { p += 1; continue; }

					if (color == PieceColor(white)) { this->whiteMoves.emplace_back(moveToAdd); }
					else { this->blackMoves.emplace_back(moveToAdd); }
				}
				if ((uint64_t)(combinedOppositeColorBoard & (1ULL << 64 - newIdx - 1)) != 0 ) { break; }
				p++;
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

	std::vector<int> indexes;

	Move moveToAdd;

	int newIdx;

	if (color == PieceColor(white)) {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(white));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(black));


		indexes = this->getBitIndexes(this->whiteKing);



	}
	else {
		combinedSameColorBoard = this->getCombinedBoard(PieceColor(black));
		combinedOppositeColorBoard = this->getCombinedBoard(PieceColor(white));

		indexes = this->getBitIndexes(this->blackKing);
	}
	if (indexes.size() == 0) { return; }

	for (int i : indexes) {
		gridCoords = this->convertGridCoords(i);

		x = gridCoords.first;
		y = gridCoords.second;
		//iterate through all the kings directions
		for (int j = 0; j < 8; j++) {
			newX = x + dx[j];
			newY = y + dy[j];

			newIdx = this->convertGridCoords(std::make_pair(newX, newY));

			//Check bounds and that it doesnt collide with same colored piece
			if ((uint64_t)(combinedSameColorBoard & (1ULL << 64 - newIdx - 1)) == 0
				&& 0 <= newX && newX < 8 && 0 <= newY && newY < 8) {
				moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(newX, newY));
				moveToAdd.eats = this->eats(moveToAdd);
				if (calculateExtended && this->wouldMoveCauseCheck(moveToAdd)) { continue; }
				if (color == PieceColor(white)) { this->whiteMoves.emplace_back(moveToAdd); }
				else { this->blackMoves.emplace_back(moveToAdd); }
			}
		}

	}

	//Take into account black castling
	gridCoords = this->convertGridCoords(indexes[0]);
	x = std::get<0>(gridCoords);
	y = std::get<1>(gridCoords);
	if (color == white) {
		newIdx = this->convertGridCoords(std::make_pair(x + 2, y));
		if (x == 4 && y == 7 && this->whiteKingCastling &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx)) == 0 &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx +1)) == 0 ) {
			Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x + 2, y));
			if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
				this->whiteMoves.emplace_back(moveToAdd);
			}
		}
		newIdx = this->convertGridCoords(std::make_pair(x - 2, y));
		if (x == 4 && y == 7 && this->whiteQueenCastling &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx)) == 0 &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx - 1)) == 0) 
		{
			Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x - 2, y));

			if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
				this->whiteMoves.emplace_back(moveToAdd);
			}
		}
	}
	else {
		newIdx = this->convertGridCoords(std::make_pair(x + 2, y));
		if (x == 4 && y == 0 && this->blackKingCastling &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx)) == 0 &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx + 1)) == 0) {
			Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x + 2, y));

			if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
				this->blackMoves.emplace_back(moveToAdd);
			}
		}
		newIdx = this->convertGridCoords(std::make_pair(x - 2, y));
		if (x == 4 && y == 0 && this->blackQueenCastling &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx)) == 0 &&
			(uint64_t)((combinedOppositeColorBoard | combinedSameColorBoard) & (1ULL << 63 - newIdx - 1)) == 0) {
			Move moveToAdd = Move(PieceType(King), PieceColor(color), std::make_pair(x, y), std::make_pair(x - 2, y));

			if (!calculateExtended || !this->wouldMoveCauseCheck(moveToAdd)) {
				this->blackMoves.emplace_back(moveToAdd);
			}
		}
	}
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
		}
		else if (to == std::make_pair(6, 0) && this->blackKingCastling) {
			rookMoveCastling = Move(Rook, black, std::make_pair(7, 0), std::make_pair(5, 0));
			this->executeMove(rookMoveCastling);
			this->toMove = this->toMove == white ? black : white;
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
		
	}
	//If kings are moved, disable castling
	else if (type == King) {
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
		if ((((uint64_t)this->getCombinedBoard(PieceColor(black)) & (1ULL << this->convertGridCoords(to))) != 0)) {
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
		if ((((uint64_t)this->getCombinedBoard(PieceColor(white)) & (1ULL << this->convertGridCoords(to))) != 0)) {
			captureMade = true;
		}
		this->whitePawns &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteKnights &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteBishops &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteQueens &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteRooks &= ~(1ULL << (63 - this->convertGridCoords(to)));
		this->whiteKing &= ~(1ULL << (63 - this->convertGridCoords(to)));

		
	}

	//update move counter
	if (captureMade) {
		this->halfMoves = 0;
	}
	else {
		this->halfMoves += 1;
	}
	this->fullMoves += 1;

	//remove Move from set
	if (color == PieceColor(white)) {
		this->whiteMoves.clear();
	}
	else {
		this->blackMoves.clear();
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
	
	this->storeOldMoves();
	this->storeEnPassantSquare();
	// Execute the move on the cloned board
	this->executeMove(move);
	// Get the position of the king after the move
	std::pair<int, int> kingPos;
	std::vector<Move> moves;
	if (move.pieceColor == white) {
		kingPos = this->getKingPosition(white);
		this->calculateBlackMoves(false);
		moves = this->blackMoves;
	}
	else {
		kingPos = this->getKingPosition(black);
		this->calculateWhiteMoves(false);
		moves = this->whiteMoves;
	}
	for (const Move& opponentMove : moves) {
		if (opponentMove.to == kingPos) {
			this->undoMove(move);
			this->restoreOldMoves();
			this->restoreEnPassantSquare();
			return true;
		}
	}

	this->undoMove(move);

	this->restoreOldMoves();
	this->restoreEnPassantSquare();

	return false;
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
		std::cout << row << " " << col << std::endl;
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
	std::vector<Move> movesToCheck;
	int bestScore = INT_MIN;
	Move bestMove;

	// Select moves based on the current player
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		movesToCheck = this->whiteMoves;
		bestScore = INT_MIN;
		  // Maximize for white
	}
	else {
		this->calculateBlackMoves();
		movesToCheck = this->blackMoves;
		bestScore = INT_MAX;// Minimize for black
	}



	int currScore;
	Chessboard tempBoard = *this;
	tempBoard.storeOldMoves();
	// Iterate through all valid moves
	for (Move move : movesToCheck) {
		tempBoard.storeEnPassantSquare();
		tempBoard.executeMove(move);
		if (tempBoard.toMove == white) {
			tempBoard.calculateWhiteMoves();
		}
		else {
			tempBoard.calculateBlackMoves();
		}
		currScore = (this->toMove == black ? 1 : -1)* tempBoard.negaMaxAlphaPruning(DEPTH-1, INT_MIN+ 2,INT_MAX -2);

		// Print the current move and score
		std::cout << "Checking root: " << move << " : " << currScore << std::endl;

		// Update best move based on the current player's strategy (maximize or minimize)
		if ((this->toMove == white && currScore >= bestScore) ||
			(this->toMove == black && currScore <= bestScore)) {
			bestMove = move;
			bestScore = currScore;
			if ((this->toMove == white && bestScore == MAX_SCORE) || (this->toMove == black && bestScore == MIN_SCORE)) {
				break;
			}
		}
		tempBoard.restoreEnPassantSquare();
		tempBoard.restoreOldMoves();
		tempBoard.undoMove(move);
	}

	// Output the best move
	std::cout << "Chosen move: " << bestMove << " : " << bestScore << std::endl;
	return bestMove;
}

int Chessboard::negaMax(int depth) {

	if (depth == 0) {
		return this->simpleEvaluation();  // Base case: return evaluation at depth 0
	}


	std::vector<Move> movesToCheck;
	int bestScore = INT_MIN;

	// Select moves based on the current player
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		movesToCheck = this->whiteMoves;
	}
	else {
		this->calculateBlackMoves();
		movesToCheck = this->blackMoves;
	}

	int currScore;
	Chessboard tempBoard;

	// If no moves are possible, it's either a loss or a draw
	if (movesToCheck.empty()) {
		return MIN_SCORE;  // Loss for the current player
	}

	// Iterate through all possible moves
	for (Move move : movesToCheck) {

		// Apply the move to a temporary board and evaluate it
		tempBoard = *this;
		tempBoard.executeMove(move);
		if (tempBoard.toMove == white) {
			tempBoard.calculateWhiteMoves();
		}
		else {
			tempBoard.calculateBlackMoves();
		}
		currScore = -tempBoard.negaMax(depth - 1);  // Negate score for alternating turns
		// Update best score based on the current player’s strategy
		if (currScore > bestScore) {
			bestScore = currScore;
		}

		if (bestScore == MAX_SCORE) {
			return bestScore;
		}
	}

	return bestScore;
}

int Chessboard::negaMaxAlphaPruning(int depth, int alpha, int beta) {
	if (depth == 0) {
		return this->simpleEvaluation();  // Base case: return evaluation at depth 0
	}

	std::vector<Move> movesToCheck;
	int bestScore = INT_MIN;

	// Select moves based on the current player
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		movesToCheck = this->whiteMoves;
	}
	else {
		this->calculateBlackMoves();
		movesToCheck = this->blackMoves;
	}

	int currScore;
	Chessboard tempBoard = *this;		

	// If no moves are possible, it's either a loss or a draw
	if (movesToCheck.empty()) {
		return MIN_SCORE;  // Loss for the current player
	}

	// Iterate through all possible moves
	for (Move& move : movesToCheck) {
		
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
		if (currScore >= beta) {
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

	//handle normal moves
	uint64_t* boardOfType = this->getBoardOfType(move.pieceType, move.pieceColor);

	//Delete its current position and add old position

	//delete "new" position
	*boardOfType &= ~(1ULL << 63 - this->convertGridCoords(move.to));

	//add old position
	*boardOfType |= (1ULL << 63 - this->convertGridCoords(move.from));



	//Check for en passant: has to be a pawn, has to have moved diagonally and cant have eaten anything
	if (move.pieceType == Pawn && abs(move.to.first - move.from.first) == 1 && !move.eats.has_value()) {
		//TODO
		uint64_t* boardOfOppositePawn = this->getBoardOfType(Pawn, move.pieceType == white ? black : white);
		std::pair<int, int> pawnCoords = move.to;
		
		if (move.pieceType == white) {
			this->blackPawns|= (1ULL << 63 - this->convertGridCoords(std::make_pair(move.to.first , move.to.second +1)));
		}
		else {
			this->whitePawns |= (1ULL << 63 - this->convertGridCoords(std::make_pair(move.to.first, move.to.second - 1)));
		}



	}

	//Check for castling: King moved horizontally more than one square
	if (move.pieceType == King && abs(move.to.first - move.from.first) != 1) {
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
		}
		this->undoMove(rookCastlingMove);
		return;
	}

	//Restore old piece
	if (move.eats.has_value()) {
		uint64_t* boardOfEatenPiece = this->getBoardOfType(move.eats.value().type, move.eats.value().color);
		*boardOfEatenPiece |= (1ULL << 63 - this->convertGridCoords(move.to));
	}
	this->toMove = this->toMove == white ? black : white;
}

int Chessboard::countMoves(int depth) {
	if (depth == 0) {
		return 1;
	}
	int ans = 0;
	std::vector<std::future<int>> futures;
	this->whiteMoves.clear();
	this->blackMoves.clear();
	std::array <Move, 256> movesToCheck;
	int moveCount;
	if (this->toMove == white) {
		this->calculateWhiteMoves();
		std::copy(this->whiteMoves.begin(), this->whiteMoves.end(), movesToCheck.begin());
		moveCount = this->whiteMoves.size();
	}
	else {
		this->calculateBlackMoves();
		std::copy(this->blackMoves.begin(), blackMoves.end(), movesToCheck.begin());
		moveCount = this->blackMoves.size();
	}
	for (int i = 0; i < moveCount; i++)
	{	

		
		this->storeEnPassantSquare();
		this->executeMove(movesToCheck[i]);
		if (depth > 3 && futures.size() < maxThreads) {
			Chessboard tempboard = *this;
			futures.push_back(std::async(std::launch::async, [tempboard, depth]() mutable {
				
				return tempboard.countMoves(depth - 1);
				}));
		}
		else {
			ans += this->countMoves(depth - 1);
		}
		this->restoreEnPassantSquare();
		this->undoMove(movesToCheck[i]);
	}

	for (auto& future : futures) {
		ans += future.get();
	}
	
	return ans;
	
}


void Chessboard::storeOldMoves() {
	this->oldWhiteMoves = this->whiteMoves;
	this->oldBlackMoves = this->blackMoves;
}

void Chessboard::restoreOldMoves() {
	this->whiteMoves = this->oldWhiteMoves;
	this->blackMoves = this->oldBlackMoves;
}

void Chessboard::storeEnPassantSquare() {
	this->oldEnPassantSquare = this->enPassantSquare;
}

void Chessboard::restoreEnPassantSquare() {
	this->enPassantSquare = this->oldEnPassantSquare;
	this->oldEnPassantSquare = std::nullopt;
}