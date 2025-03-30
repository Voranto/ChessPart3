#include "MultiplayerChessGUI.h"
#include "ChessGUI.h"

MultiplayerChessGUI::MultiplayerChessGUI(PieceColor color, Chessboard board) :ChessGUI(GUI_SCREENS(MULTIPLAYER_JOIN), board) {
	this->clientColor = color;
}

void MultiplayerChessGUI::drawPieces(sf::RenderWindow& window, sf::Vector2i offset) {
	this->drawPawns(white, window, offset);
	this->drawPawns(black, window, offset);
	this->drawKnights(white, window, offset);
	this->drawKnights(black, window, offset);
	this->drawBishops(white, window, offset);
	this->drawBishops(black, window, offset);
	this->drawRooks(white, window, offset);
	this->drawRooks(black, window, offset);
	this->drawQueens(white, window, offset);
	this->drawQueens(black, window, offset);
	this->drawKing(white, window, offset);
	this->drawKing(black, window, offset);

}

void MultiplayerChessGUI::drawKnights(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteKnights;
		currSprite = this->whiteKnightSprite;
	}
	else {
		currBoard = this->chessboard.blackKnights;
		currSprite = this->blackKnightSprite;
	}

	std::vector<int> indexes = this->chessboard.getBitIndexes(currBoard);

	std::pair<int, int> gridCoords;
	int x;
	int y;


	for (auto& idx : indexes) {
		gridCoords = this->convertCoordsByColor(this->chessboard.convertGridCoords(idx));
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		if (gridCoords == this->convertCoordsByColor(this->selectedPiece.pos)) { continue; }
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawPawns(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite;

	if (color == white) {
		currBoard = this->chessboard.whitePawns;
		currSprite = this->whitePawnSprite;
	}
	else {
		currBoard = this->chessboard.blackPawns;
		currSprite = this->blackPawnSprite;
	}

	std::vector<int> indexes = this->chessboard.getBitIndexes(currBoard);

	std::pair<int, int> gridCoords;
	int x;
	int y;


	for (auto& idx : indexes) {
		gridCoords = this->convertCoordsByColor(this->chessboard.convertGridCoords(idx));
		if (gridCoords == this->convertCoordsByColor(this->selectedPiece.pos)) { continue; }
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawBishops(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteBishops;
		currSprite = this->whiteBishopSprite;
	}
	else {
		currBoard = this->chessboard.blackBishops;
		currSprite = this->blackBishopSprite;
	}

	std::vector<int> indexes = this->chessboard.getBitIndexes(currBoard);

	std::pair<int, int> gridCoords;
	int x;
	int y;


	for (auto& idx : indexes) {
		gridCoords = this->convertCoordsByColor(this->chessboard.convertGridCoords(idx));
		if (gridCoords == this->convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawRooks(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteRooks;
		currSprite = this->whiteRookSprite;
	}
	else {
		currBoard = this->chessboard.blackRooks;
		currSprite = this->blackRookSprite;
	}

	std::vector<int> indexes = this->chessboard.getBitIndexes(currBoard);

	std::pair<int, int> gridCoords;
	int x;
	int y;


	for (auto& idx : indexes) {
		gridCoords = this->convertCoordsByColor(this->chessboard.convertGridCoords(idx));
		if (gridCoords == this->convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawKing(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteKing;
		currSprite = this->whiteKingSprite;
	}
	else {
		currBoard = this->chessboard.blackKing;
		currSprite = this->blackKingSprite;
	}

	std::vector<int> indexes = this->chessboard.getBitIndexes(currBoard);

	std::pair<int, int> gridCoords;
	int x;
	int y;


	for (auto& idx : indexes) {
		gridCoords = this->convertCoordsByColor(this->chessboard.convertGridCoords(idx));
		if (gridCoords == this->convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void MultiplayerChessGUI::drawQueens(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
	uint64_t currBoard;
	sf::Sprite currSprite;

	if (color == white) {
		currBoard = this->chessboard.whiteQueens;
		currSprite = this->whiteQueenSprite;
	}
	else {
		currBoard = this->chessboard.blackQueens;
		currSprite = this->blackQueenSprite;
	}

	std::vector<int> indexes = this->chessboard.getBitIndexes(currBoard);

	std::pair<int, int> gridCoords;
	int x;
	int y;


	for (auto& idx : indexes) {
		gridCoords = this->convertCoordsByColor(this->chessboard.convertGridCoords(idx));
		if (gridCoords == this->convertCoordsByColor(this->selectedPiece.pos)) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

std::pair<int, int> MultiplayerChessGUI::convertCoordsByColor(std::pair<int, int> coords) {
	if (this->clientColor == white) {
		return coords;
	}

	return std::make_pair(7 - std::get<0>(coords), 7 -std::get<1>(coords));
}


void MultiplayerChessGUI::drawSelectedPiece(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	//Firewall incase there is no selected Piece
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::Sprite spriteToDraw = this->getSpriteOfPiece(this->selectedPiece.type, this->selectedPiece.color);
	//If it just has been clicked, render normally
	sf::Vector2i vectorPos = sf::Vector2i(std::get<0>(this->convertCoordsByColor(this->selectedPiece.pos)), std::get<1>(this->convertCoordsByColor(this->selectedPiece.pos)));
	vectorPos *= this->squareSize;
	vectorPos += boardOffset;
	spriteToDraw.setScale(2, 2);
	if (clickEvent == 1) {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}
	else if (clickEvent == 3) {
		spriteToDraw.setPosition(sf::Vector2f(sf::Mouse::getPosition().x - this->squareSize / 2, sf::Mouse::getPosition().y - this->squareSize / 2));
		window.draw(spriteToDraw);
	}
	else {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}

}

void MultiplayerChessGUI::setSelectedPiece(std::pair<int, int> gridPos, PieceColor color) {
	int oneDimensionalIndex = this->chessboard.convertGridCoords(gridPos);

	

	uint64_t valueToCompare = (1ULL << 63 - oneDimensionalIndex);
	if (color == white) {
		if ((this->chessboard.whiteKnights & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Knight);
		}
		else if ((this->chessboard.whiteBishops & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Bishop);
		}
		else if ((this->chessboard.whitePawns & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Pawn);
		}
		else if ((this->chessboard.whiteRooks & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Rook);
		}
		else if ((this->chessboard.whiteQueens & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Queen);
		}
		else if ((this->chessboard.whiteKing & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, King);
		}
	}
	else {
		if ((this->chessboard.blackKnights & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Knight);
		}
		else if ((this->chessboard.blackBishops & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Bishop);
		}
		else if ((this->chessboard.blackPawns & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Pawn);
		}
		else if ((this->chessboard.blackRooks & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Rook);
		}
		else if ((this->chessboard.blackQueens & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, Queen);
		}
		else if ((this->chessboard.blackKing & valueToCompare) != 0) {
			this->selectedPiece = Piece(gridPos, color, King);
		}
	}
	

}

std::optional<Move> MultiplayerChessGUI::processClick(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {

	if (this->chessboard.toMove != this->clientColor) { return std::nullopt; }

	//Mouse position data
	sf::Vector2i mousePos = sf::Mouse::getPosition();
	sf::Vector2i centeredMousePos = mousePos - boardOffset;

	std::pair<int, int> gridPos = std::make_pair(centeredMousePos.x / this->squareSize, centeredMousePos.y / this->squareSize);



	//Draw selected piece according to clickevent
	if (clickEvent == 1) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		std::pair<int, int> gridClick = this->convertCoordsByColor(getGridOfClick(mousePos, boardOffset));

		int oneDimensionalClick = this->chessboard.convertGridCoords(gridClick);

		uint64_t combinedSameColorBoard = this->chessboard.getCombinedBoard(this->clientColor);
		uint64_t combinedOppositeColorBoard = this->chessboard.getCombinedBoard(this->clientColor == white ? black : white);


		//If no selected Piece, check for selection
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) {
			if ((combinedSameColorBoard & (1ULL << 63 - oneDimensionalClick)) != 0) {
				std::cout << std::get<0>(gridClick) << std::get<1>(gridClick) << std::endl;
				this->setSelectedPiece(gridClick, this->clientColor);
				std::cout << "Selected Piece multiplayer: " << this->selectedPiece << std::endl;
			}
		}
		else if (this->selectedPiece.pos != std::make_pair(-1, -1)) {

			//Clicking in and empty square or clicking in square of opposite color
			if ((((combinedSameColorBoard) & (1ULL << 63 - oneDimensionalClick)) == 0)) {
				Move attemptedMove = Move(this->selectedPiece.type, this->selectedPiece.color, this->selectedPiece.pos, this->convertCoordsByColor(gridPos));

				if (this->isValidMove(attemptedMove)) {
					this->chessboard.executeMove(attemptedMove);
					
					this->clearSelectedPiece();

					if (this->isTherePromotion(attemptedMove)) {
						this->handlePromotions(attemptedMove, window);
						std::cout << "ended promotion" << std::endl;
					}

					if (this->chessboard.toMove == white) {
						this->chessboard.calculateWhiteMoves();
						if (this->chessboard.whiteMoves.size() == 0) {
							std::cout << "CHECKMATE: BLACK WINS" << std::endl;
						}
					}
					else {
						this->chessboard.calculateBlackMoves();
						if (this->chessboard.blackMoves.size() == 0) {
							std::cout << "CHECKMATE: WHITE WINS" << std::endl;
						}
					}
					return attemptedMove;
				}
				else {

					this->clearSelectedPiece();
				}
			}
			if (((combinedSameColorBoard) & (1ULL << 63 - oneDimensionalClick)) != 0) {
				this->setSelectedPiece(gridClick, this->clientColor);
			}
		}
	}
	//Handle stuff that happens when RELEASING mouse button
	else if (clickEvent == 2) {
		//In case no selected piece, release of click has no use
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return std::nullopt; }



		if (centeredMousePos.x >= 0 && centeredMousePos.x <= this->squareSize * 8 &&
			centeredMousePos.y >= 0 && centeredMousePos.y <= this->squareSize * 8) {
			Move attemptedMove = Move(this->selectedPiece.type, this->selectedPiece.color, this->selectedPiece.pos, this->convertCoordsByColor(gridPos));
			std::cout << "attemptedMove: " << attemptedMove << std::endl;

			if (this->isValidMove(attemptedMove)) {

				this->chessboard.executeMove(attemptedMove);

				this->clearSelectedPiece();

				if (this->isTherePromotion(attemptedMove)) {
					this->handlePromotions(attemptedMove, window);
					std::cout << "ended promotion" << std::endl;
				}

				if (this->chessboard.toMove == white) {
					this->chessboard.calculateWhiteMoves();
					if (this->chessboard.whiteMoves.size() == 0) {
						std::cout << "CHECKMATE: BLACK WINS" << std::endl;
					}
				}
				else {
					this->chessboard.calculateBlackMoves();
					if (this->chessboard.blackMoves.size() == 0) {
						std::cout << "CHECKMATE: WHITE WINS" << std::endl;
					}
				}
				return attemptedMove;


			}

		}

	}
	return std::nullopt;
}

void MultiplayerChessGUI::drawSelectedPieceSquare(sf::RenderWindow& window, sf::Vector2i boardOffset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	rect.setFillColor(sf::Color(255, 0, 0));
	std::pair<int, int> gridPos = this->convertCoordsByColor(this->selectedPiece.pos);
	sf::Vector2f newPosition = sf::Vector2f(boardOffset) + sf::Vector2f(std::get<0>(gridPos) * this->squareSize, std::get<1>(gridPos) * this->squareSize);
	rect.setPosition(newPosition);
	window.draw(rect);
}

void MultiplayerChessGUI::drawSelectedPiecePossibilities(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }

	std::vector<Move> movesToCompare;
	if (this->clientColor == white) {
		movesToCompare = this->chessboard.whiteMoves;
	}
	else {
		movesToCompare = this->chessboard.blackMoves;
	}
	sf::Vector2f circlePos;
	for (const Move& move : movesToCompare) {
		if (move.from == this->selectedPiece.pos) {
			std::pair<int, int> gridPos = this->convertCoordsByColor(move.to);
			circlePos = sf::Vector2f(std::get<0>(gridPos) * this->squareSize, std::get<1>(gridPos) * this->squareSize);
			circlePos += sf::Vector2f(boardOffset);
			circlePos += sf::Vector2f(40, 40);
			this->drawCircle(window, circlePos, 20, sf::Color(100, 100, 100));
		}
	}
}
