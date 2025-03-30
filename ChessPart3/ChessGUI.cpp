#include "ChessGUI.h"
ChessGUI::ChessGUI(GUI_SCREENS screen, Chessboard board) {
	this->mode = screen;
	this->chessboard = board;

	this->squareSize = 120;
	this->selectedPiece = Piece(-1,-1,white,Pawn);

	if (!whitePawnTexture.loadFromFile(".\\img\\whitePawn.png")) {
		std::cerr << "Failed to load whitePawn.png" << std::endl;
	}
	this->whitePawnSprite = sf::Sprite(whitePawnTexture);
	blackPawnTexture.loadFromFile(".\\img\\blackPawn.png");
	this->blackPawnSprite = sf::Sprite(blackPawnTexture);
	whiteKnightTexture.loadFromFile(".\\img\\whiteKnight.png");
	this->whiteKnightSprite = sf::Sprite(whiteKnightTexture);
	blackKnightTexture.loadFromFile(".\\img\\blackKnight.png");
	this->blackKnightSprite = sf::Sprite(blackKnightTexture);
	whiteBishopTexture.loadFromFile(".\\img\\whiteBishop.png");
	this->whiteBishopSprite = sf::Sprite(whiteBishopTexture);
	blackBishopTexture.loadFromFile(".\\img\\blackBishop.png");
	this->blackBishopSprite = sf::Sprite(blackBishopTexture);
	whiteRookTexture.loadFromFile(".\\img\\whiteRook.png");
	this->whiteRookSprite = sf::Sprite(whiteRookTexture);
	blackRookTexture.loadFromFile(".\\img\\blackRook.png");
	this->blackRookSprite = sf::Sprite(blackRookTexture);
	whiteQueenTexture.loadFromFile(".\\img\\whiteQueen.png");
	this->whiteQueenSprite = sf::Sprite(whiteQueenTexture);
	blackQueenTexture.loadFromFile(".\\img\\blackQueen.png");
	this->blackQueenSprite = sf::Sprite(blackQueenTexture);
	whiteKingTexture.loadFromFile(".\\img\\whiteKing.png");
	this->whiteKingSprite = sf::Sprite(whiteKingTexture);
	blackKingTexture.loadFromFile(".\\img\\blackKing.png");
	this->blackKingSprite = sf::Sprite(blackKingTexture);
}

void ChessGUI::drawChessBoard(sf::RenderWindow& window, sf::Vector2i offset) {
	
	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	for (int i = 0; i < 8; i ++) {
		for (int j = 0; j < 8 ; j ++) {
			if ((i + j) % 2 != 0) {
				rect.setFillColor(sf::Color(37, 82, 36));
			}
			else {
				rect.setFillColor(sf::Color(222, 218, 146));
			}
			rect.setPosition(offset.x + this->squareSize * j, offset.y + this->squareSize * i);
			window.draw(rect);
		}
	}
}

void ChessGUI::drawSelectedPieceSquare(sf::RenderWindow& window, sf::Vector2i offset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::RectangleShape rect(sf::Vector2f(this->squareSize, this->squareSize));
	rect.setFillColor(sf::Color(255, 0, 0));
	sf::Vector2f newPosition = sf::Vector2f(offset) + sf::Vector2f(std::get<0>(this->selectedPiece.pos) * this->squareSize , std::get<1>(this->selectedPiece.pos) * this->squareSize);
	rect.setPosition(newPosition);
	window.draw(rect);
}

void ChessGUI::drawPieces(sf::RenderWindow& window, sf::Vector2i offset) {
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

void ChessGUI::drawKnights(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = this->chessboard.convertGridCoords(idx);
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		if (gridCoords == this->selectedPiece.pos) { continue; }
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x* this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawPawns(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = this->chessboard.convertGridCoords(idx);
		if (gridCoords == this->selectedPiece.pos) { continue; }
		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawBishops(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = this->chessboard.convertGridCoords(idx);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawRooks(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = this->chessboard.convertGridCoords(idx);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawKing(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = this->chessboard.convertGridCoords(idx);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

void ChessGUI::drawQueens(PieceColor color, sf::RenderWindow& window, sf::Vector2i offset) {
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
		gridCoords = this->chessboard.convertGridCoords(idx);
		if (gridCoords == this->selectedPiece.pos) { continue; }

		x = std::get<0>(gridCoords);
		y = std::get<1>(gridCoords);
		currSprite.setScale(sf::Vector2f(2, 2));
		currSprite.setPosition(sf::Vector2f(offset) + sf::Vector2f(x * this->squareSize, y * this->squareSize));
		window.draw(currSprite);
	}
}

std::pair<int, int> ChessGUI::getGridOfClick(sf::Vector2i mousePos, sf::Vector2i offset) {
	mousePos -= offset;

	if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x > 8 * this->squareSize || mousePos.y > 8 * this->squareSize) {
		return std::make_pair(-1, -1);
	}
	return std::make_pair(std::trunc(mousePos.x / this->squareSize), std::trunc(mousePos.y / this->squareSize));
}

void ChessGUI::processClick(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	
	//Mouse position data
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2i centeredMousePos = mousePos - boardOffset;

	std::pair<int, int> gridPos = std::make_pair(centeredMousePos.x / this->squareSize, centeredMousePos.y / this->squareSize);


	//Draw selected piece according to clickevent
	if (clickEvent == 1) {
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		std::pair<int, int> gridClick = getGridOfClick(mousePos, boardOffset);

		int oneDimensionalClick = this->chessboard.convertGridCoords(gridClick);

		uint64_t combinedSameColorBoard = this->chessboard.getCombinedBoard(this->chessboard.toMove);
		uint64_t combinedOppositeColorBoard = this->chessboard.getCombinedBoard(this->chessboard.toMove == white ? black : white);
		
		
		//If no selected Piece, check for selection
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) {
			if ((combinedSameColorBoard & (1ULL << 63 - oneDimensionalClick)) != 0) {
				this->setSelectedPiece(gridClick, this->chessboard.toMove);
				
				std::cout <<"Selected Piece: " <<  this->selectedPiece << std::endl;
			}
		}
		else if (this->selectedPiece.pos != std::make_pair(-1, -1)) {

			//Clicking in and empty square or clicking in square of opposite color
			if ((((combinedSameColorBoard ) & (1ULL << 63 - oneDimensionalClick)) == 0 )) {
				Move attemptedMove = Move(this->selectedPiece.type, this->selectedPiece.color, this->selectedPiece.pos, gridPos);

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
					
				}
				else {

					this->clearSelectedPiece();
				}
			}
			if (((combinedSameColorBoard) & (1ULL << 63 - oneDimensionalClick)) != 0) {
				this->setSelectedPiece(gridClick, this->chessboard.toMove);
			}
		}
	}
	//Handle stuff that happens when RELEASING mouse button
	else if (clickEvent == 2) {
		//In case no selected piece, release of click has no use
		if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }


		
		if (centeredMousePos.x >= 0 && centeredMousePos.x <= this->squareSize * 8 &&
			centeredMousePos.y >= 0 && centeredMousePos.y <= this->squareSize * 8) {
			Move attemptedMove = Move(this->selectedPiece.type, this->selectedPiece.color, this->selectedPiece.pos, gridPos);


			if(this->isValidMove(attemptedMove) ){
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

				
			}

		}

	}
}

bool ChessGUI::isValidMove(Move move) {
	std::vector<Move> movesToCompare;

	if (this->chessboard.toMove == white) {
		movesToCompare = this->chessboard.whiteMoves;
	}
	else {
		movesToCompare = this->chessboard.blackMoves;
	}

	for (const Move& mv : movesToCompare) {
		if (move.equals(mv)) {
			return true;
		}
	}
	return false;

}


void ChessGUI::setSelectedPiece(std::pair<int, int> gridPos, PieceColor color) {
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

void ChessGUI::clearSelectedPiece() {

	this->selectedPiece = Piece(-1,-1, white, Pawn);
}

sf::Sprite ChessGUI::getSpriteOfPiece(PieceType type, PieceColor color) {
	switch (type) {
	case Pawn: 
		return color == white ? this->whitePawnSprite : this->blackPawnSprite;
		break;
	case Knight:
		return color == white ? this->whiteKnightSprite : this->blackKnightSprite;
		break;
	case Bishop:
		return color == white ? this->whiteBishopSprite : this->blackBishopSprite;
		break;
	case Rook:
		return color == white ? this->whiteRookSprite : this->blackRookSprite;
		break;
	case Queen:
		return color == white ? this->whiteQueenSprite : this->blackQueenSprite;
		break;
	case King:
		return color == white ? this->whiteKingSprite : this->blackKingSprite;
		break;
	}
}

void ChessGUI::drawSelectedPiece(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	//Firewall incase there is no selected Piece
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }
	sf::Sprite spriteToDraw = this->getSpriteOfPiece(this->selectedPiece.type, this->selectedPiece.color);
	//If it just has been clicked, render normally
	sf::Vector2i vectorPos = sf::Vector2i(std::get<0>(this->selectedPiece.pos), std::get<1>(this->selectedPiece.pos));
	vectorPos *= this->squareSize;
	vectorPos += boardOffset;
	spriteToDraw.setScale(2,2);
	if (clickEvent == 1) {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}
	else if (clickEvent == 3) {
		spriteToDraw.setPosition(sf::Vector2f(sf::Mouse::getPosition(window).x - this->squareSize / 2 , sf::Mouse::getPosition(window).y - this->squareSize / 2) );
		window.draw(spriteToDraw);
	}
	else {
		spriteToDraw.setPosition(sf::Vector2f(vectorPos));
		window.draw(spriteToDraw);
	}
	
}

void ChessGUI::drawSelectedPiecePossibilities(int clickEvent, sf::RenderWindow& window, sf::Vector2i boardOffset) {
	if (this->selectedPiece.pos == std::make_pair(-1, -1)) { return; }

	std::vector<Move> movesToCompare;
	if (this->chessboard.toMove == white) {
		movesToCompare = this->chessboard.whiteMoves;
	}
	else {
		movesToCompare = this->chessboard.blackMoves;
	}
	sf::Vector2f circlePos;
	for (const Move& move : movesToCompare) {
		if (move.from == this->selectedPiece.pos) {
			circlePos = sf::Vector2f(std::get<0>(move.to) * this->squareSize, std::get<1>(move.to) * this->squareSize);
			circlePos += sf::Vector2f(boardOffset);
			circlePos += sf::Vector2f(40, 40);
			this->drawCircle(window, circlePos,  20, sf::Color(100, 100, 100));
		}
	}
}

void ChessGUI::handlePromotions(Move& move, sf::RenderWindow& window) {
	//Create buttons
	Button queenButton = Button(sf::Vector2f(250, 100), this->font, "QUEEN", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	Button rookButton = Button(sf::Vector2f(250, 200), this->font, "ROOK", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	Button knightButton = Button(sf::Vector2f(250, 300), this->font, "KNIGHT", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	Button bishopButton = Button(sf::Vector2f(250, 400), this->font, "BISHOP", 20, sf::Color(220, 220, 220), sf::Color(150, 150, 150), 20);
	
	this->buttons.emplace_back(queenButton);
	this->buttons.emplace_back(knightButton);
	this->buttons.emplace_back(rookButton);
	this->buttons.emplace_back(bishopButton);



	int numberOfButtons = this->buttons.size();

	sf::Vector2i boardOffset(500, 50);

	std::optional<Button> clickedButton = std::nullopt;
	int clickEvent = -1;

	Piece pieceToReturn;
	
	//Loop until button clicked
	while (true) {
		window.clear();
		clickEvent = updateClickEvent(clickEvent, processEventsAndReturnOnClick(window));


		sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

		this->processClick(clickEvent, window, boardOffset);

		this->drawChessBoard(window, boardOffset);

		this->drawSelectedPieceSquare(window, boardOffset);


		this->drawPieces(window, boardOffset);

		this->drawSelectedPiece(clickEvent, window, boardOffset);

		this->drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

		clickedButton = this->renderButtons(window, clickEvent == 1);

		if (clickedButton.has_value()) {
			if (clickedButton.value().textString == "PRINT FEN") {
				std::cout << this->chessboard.getFEN() << std::endl;
			}
			if (clickedButton.value().textString == "QUEEN") {
				pieceToReturn =  Piece(std::make_pair(-1, -1), move.pieceColor, Queen);
				break;
			}
			if (clickedButton.value().textString == "ROOK") {
				pieceToReturn = Piece(std::make_pair(-1, -1), move.pieceColor, Rook);
				break;
			}
			if (clickedButton.value().textString == "BISHOP") {
				pieceToReturn = Piece(std::make_pair(-1, -1), move.pieceColor, Bishop);
				break;
			}
			if (clickedButton.value().textString == "KNIGHT") {
				pieceToReturn = Piece(std::make_pair(-1, -1), move.pieceColor, Knight);
				break;
			}

		}

		window.display();
	}
	//Delete the buttons
	for (int i = 0;i < 4;i++) {
		this->buttons.pop_back();
	}
	
	//Delete pawn from board
	uint64_t* pawnBoard = this->chessboard.getBoardOfType(Pawn, move.pieceColor);
	int oneDimensionalIndex = this->chessboard.convertGridCoords(move.to);
	*pawnBoard &= ~(1ULL << 63 - oneDimensionalIndex);
	

	//Add piece of new type
	uint64_t* pieceBoard = this->chessboard.getBoardOfType(pieceToReturn.type, pieceToReturn.color);
	*pieceBoard |= (1ULL << 63 - oneDimensionalIndex);
}

bool ChessGUI::isTherePromotion(Move& move) {
	if (move.pieceType == Pawn) {
		if (move.pieceColor == black && std::get<1>(move.to) == 7) {
			return true;
		}
		if (move.pieceColor == white && std::get<1>(move.to) == 0) {
			return true;
		}
	}
	return false;
}



