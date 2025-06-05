// System-specific defines — goes first!
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Standard Library Headers
#include <iostream>
#include <optional>
#include <bitset>
#include <chrono>
#include <thread>
#include <cmath>       // prefer <cmath> over <math.h>
#include <sstream>
#include <cstdlib>
#include <fstream>



// SFML
#include <SFML/Network.hpp>          // comes before SFML/Graphics.hpp if using both
#include <SFML/Graphics.hpp>

// Your own headers (local includes last)
#include "GUI.h"
#include "Button.h"
#include "GUI_Screens.h"
#include "ChessGUI.h"
#include "MultiplayerChessGUI.h"
#include "TextBox.h"


sf::RenderWindow window;
sf::Vector2f windowSize;

sf::Font font;

bool DEBUG = true;

void renderStartGUI();
void renderSingleplayerGUI();
void renderHostGameGUI();
void renderLocalGameGUI();
void renderBotGUI();
std::vector<uint8_t> serializeMove(const Move& move);
Move deserializeMove(const uint8_t* data, size_t dataLength);
void renderJoinGameGUI();

void  renderMultiplayerGameGUI(PieceColor color, sf::TcpSocket* socket);



void renderStartGUI() {
    
    int clickEvent;
    // Load from a font file on disk
    
    sf::Texture background;
    background.loadFromFile(".\\img\\ChessWallpaper.png");

    GUI startGUI = GUI(GUI_SCREENS(START));
    startGUI.setBackground(background, windowSize);
    Button singleplayerButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.3), font, "SINGLEPLAYER", 70, sf::Color(255, 255, 255), sf::Color(200,200,200), 80);
    startGUI.buttons.emplace_back(singleplayerButton);

    Button hostGameButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.5), font, "HOST GAME", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    startGUI.buttons.emplace_back(hostGameButton);

    Button joinGameButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.7), font, "JOIN GAME", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    startGUI.buttons.emplace_back(joinGameButton);
    

    std::optional<Button> clickedButton;

    while (window.isOpen())
    {


        clickEvent = startGUI.processEventsAndReturnOnClick(window);

        window.clear();


        startGUI.drawBackground(window);

        clickedButton = startGUI.renderButtons(window,clickEvent == 2 ? true : false);

        startGUI.renderTextBoxes(window, clickEvent == 2 ? true : false);

        window.display();

        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "SINGLEPLAYER") {
                std::cout << "SINGLEPLAYER" << std::endl;
                renderSingleplayerGUI();
            }
            else if (clickedButton.value().textString == "HOST GAME") {
                std::cout << "HOST GAME" << std::endl;
                renderHostGameGUI();
            }
            else if (clickedButton.value().textString == "JOIN GAME") {
                std::cout << "JOIN GAME" << std::endl;
                renderJoinGameGUI();
            }
            break;
        }

       
    }
}

void renderSingleplayerGUI() {
    int clickEvent;
    sf::Texture background;
    background.loadFromFile(".\\img\\ChessWallpaper.png");

    GUI singlePlayerGUI = GUI(GUI_SCREENS(SINGLEPLAYER));
    singlePlayerGUI.setBackground(background, windowSize);
    Button playLocallyButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.4), font, "PLAY LOCALLY", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    singlePlayerGUI.buttons.emplace_back(playLocallyButton);

    Button playBotButton = Button(sf::Vector2f(windowSize.x * 0.2, windowSize.y * 0.6), font, "PLAY AGAINST BOT", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    singlePlayerGUI.buttons.emplace_back(playBotButton);
    std::optional<Button> clickedButton;
    while (window.isOpen())
    {
        clickEvent = singlePlayerGUI.processEventsAndReturnOnClick(window);

        window.clear();


        singlePlayerGUI.drawBackground(window);

        clickedButton = singlePlayerGUI.renderButtons(window, clickEvent == 2 ? true : false);
        window.display();

        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "PLAY LOCALLY") {
                std::cout << "PLAY LOCALLY" << std::endl;
                renderLocalGameGUI();
            }
            else if (clickedButton.value().textString == "PLAY AGAINST BOT") {
                std::cout << "PLAY AGAINST BOT" << std::endl;
                renderBotGUI();
            }
            break;
        }


    }
}

void renderBotGUI() {
    int clickEvent = -1;
    int newEvent;
    Chessboard board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    MultiplayerChessGUI botGUI = MultiplayerChessGUI(white, board);
    botGUI.font = font;
    //FEATURES BUTTONS
    Button printFENButton = Button(sf::Vector2f(windowSize.x * 0.88, windowSize.y * 0.5), font, "PRINT FEN", 50, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    botGUI.buttons.emplace_back(printFENButton);

    botGUI.chessboard.calculateMoves();

    sf::Vector2i boardOffset(500, 50);

    std::optional<Button> clickedButton = std::nullopt;

    while (window.isOpen())
    {


        window.clear();
        clickEvent = botGUI.updateClickEvent(clickEvent, botGUI.processEventsAndReturnOnClick(window));


        sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

        if (botGUI.chessboard.toMove == botGUI.clientColor) {
            //botGUI.processClick(clickEvent, window, boardOffset);
            Move moveToMake = botGUI.chessboard.calculateBestMove();
            botGUI.chessboard.executeMove(moveToMake);
            if (botGUI.clientColor == black) {
                botGUI.chessboard.calculateWhiteMoves();
            }
            else {
                botGUI.chessboard.calculateBlackMoves();
            }
        }
        else {
            Move moveToMake = botGUI.chessboard.calculateBestMove();
            botGUI.chessboard.executeMove(moveToMake);
            if (botGUI.clientColor == white) {
                botGUI.chessboard.calculateWhiteMoves();
            }
            else {
                botGUI.chessboard.calculateBlackMoves();
            }
        }
        

        botGUI.drawChessBoard(window, boardOffset);

        botGUI.drawSelectedPieceSquare(window, boardOffset);


        botGUI.drawPieces(window, boardOffset);

        botGUI.drawSelectedPiece(clickEvent, window, boardOffset);

        botGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = botGUI.renderButtons(window, clickEvent == 1);

        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "PRINT FEN") {
                std::cout << botGUI.chessboard.getFEN() << std::endl;
                std::cout << botGUI.chessboard.simpleEvaluation() << std::endl;
            }
        }

        window.display();


    }
}

void renderLocalGameGUI() {
    int clickEvent = -1;
    int newEvent;
    Chessboard board("2bq3r/pppppkpp/1r3n2/1Pn2p2/4P3/PQN2N2/1P1P1PPP/R1B1KB1R b KQ-- - 0 2");
    
    ChessGUI localGameGUI = ChessGUI(GUI_SCREENS(SINGLEPLAYER_LOCAL), board);
    localGameGUI.font = font;
    //FEATURES BUTTONS
    Button printFENButton = Button(sf::Vector2f(windowSize.x * 0.88, windowSize.y * 0.5), font, "PRINT FEN", 50, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);
    localGameGUI.buttons.emplace_back(printFENButton);
    localGameGUI.chessboard.calculateBlackMoves();
    


    sf::Vector2i boardOffset(500, 50);

    std::optional<Button> clickedButton = std::nullopt;

    while (window.isOpen())
    {
        
        
        window.clear();
        clickEvent = localGameGUI.updateClickEvent(clickEvent, localGameGUI.processEventsAndReturnOnClick(window));
        

        sf::Vector2i currentCoords = sf::Mouse::getPosition(window);
        
        localGameGUI.processClick(clickEvent,window,boardOffset);
        
        localGameGUI.drawChessBoard(window, boardOffset);

        localGameGUI.drawSelectedPieceSquare(window, boardOffset);


        localGameGUI.drawPieces(window, boardOffset);

        localGameGUI.drawSelectedPiece(clickEvent, window, boardOffset);

        localGameGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = localGameGUI.renderButtons(window, clickEvent == 1);

        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "PRINT FEN") {
                std::cout << localGameGUI.chessboard.getFEN() << std::endl;
            }
        }

        window.display();


    }
}

// ---------- renderHostGameGUI ----------
void renderHostGameGUI() {

    sf::TcpListener listener;
    if (listener.listen(7777) != sf::Socket::Done) {
        std::cerr << "Failed to start the listener!" << std::endl;
        return;
    }
    std::cout << "Server is listening on port 7777..." << std::endl;
    // Accept a client connection
    sf::TcpSocket client;
    GUI hostGameGUI = GUI(MULTIPLAYER_HOST);
    sf::Texture background;
    background.loadFromFile(".\\img\\ChessWallpaper.png");
    hostGameGUI.setBackground(background, windowSize);
    sf::Text joinGuideString("", font, 25);
    joinGuideString.setString("To host a game, you have to use ngrok \n"
        "To activate ngrok, download it, go to its directory and write \n"
        " \"  ngrok tcp 7777 \" in the command line. After that, \n"
        "copy the ip adress and port and send it to your friend. \n"
        "It should look something like tcp://2.tcp.eu.ngrok.io:16886, \n"
        "where 2.tcp.eu.ngrok.io is the ip and 16886 is the port \n"
        "In the first text box  have your friend write the ip adress, in the second one the port, and click join");
    joinGuideString.setPosition(sf::Vector2f(400, 100));

    sf::RectangleShape guideRect(sf::Vector2f(joinGuideString.getGlobalBounds().width + 20, joinGuideString.getGlobalBounds().height + 20));
    guideRect.setPosition(joinGuideString.getPosition() - sf::Vector2f(10, 10));
    guideRect.setFillColor(sf::Color(50, 50, 50));

    


    if (listener.accept(client) != sf::Socket::Done) {
        std::cerr << "Failed to accept the client connection!" << std::endl;
        return;
    }
    std::cout << "Client connected!" << std::endl;
    const std::string message = "Hello from the server!";
    if (client.send(message.c_str(), message.size() + 1) != sf::Socket::Done) {
        std::cerr << "Failed to send message!" << std::endl;
    }

    // Receive data from the client
    char buffer[128];
    std::size_t received;
    if (client.receive(buffer, sizeof(buffer), received) != sf::Socket::Done) {
        std::cerr << "Failed to receive message!" << std::endl;
    }
    std::cout << "Received from client: " << buffer << std::endl;
    renderMultiplayerGameGUI(white, &client);
}


// ---------- renderJoinGameGUI ----------
void renderJoinGameGUI() {

    int clickEvent;
    sf::Texture background;
    if (!background.loadFromFile(".\\img\\ChessWallpaper.png")) {
        std::cerr << "Failed to load background image." << std::endl;
    }

    GUI joinGameGUI = GUI(GUI_SCREENS(MULTIPLAYER_JOIN));
    joinGameGUI.setBackground(background, windowSize);

 

    // Connect to the server host
    
    
    TextBox ipAddressBox(sf::Vector2f(760, 400), sf::Vector2f(400, 100), sf::Color(255, 255, 255));
    ipAddressBox.fontSize = 50;
    ipAddressBox.text = "";
    joinGameGUI.textBoxes.emplace_back(ipAddressBox);
    

    TextBox portBox(sf::Vector2f(760, 510), sf::Vector2f(400, 100), sf::Color(255, 255, 255));
    portBox.fontSize = 50;
    portBox.text = "";
    joinGameGUI.textBoxes.emplace_back(portBox);

    Button ipAddressJoin = Button(sf::Vector2f(880, 650), font, "Join", 70, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 30.0);
    joinGameGUI.buttons.emplace_back(ipAddressJoin);

    sf::Text joinGuideString("",font,25);
    joinGuideString.setString("To join a friends game, ensure that they have hosted a game and that they have ngrok active. \n"
        "To activate ngrok, download it, go to its directory and write \n"
        " \"  ngrok tcp 7777 \" in the command line. After that, \n"
        "have your friend copy the ip adress and port and send it to you. \n"
        "It should look something like tcp://2.tcp.eu.ngrok.io:16886, \n"
        "where 2.tcp.eu.ngrok.io is the ip and 16886 is the port \n"
        "In the first text box write the ip adress, in the second one the port, and click join");
    joinGuideString.setPosition(sf::Vector2f(400, 100));

    sf::RectangleShape guideRect(sf::Vector2f(joinGuideString.getGlobalBounds().width + 20, joinGuideString.getGlobalBounds().height + 20));
    guideRect.setPosition(joinGuideString.getPosition() - sf::Vector2f(10,10));
    guideRect.setFillColor(sf::Color(50, 50, 50));
    
    bool connected = false;
    bool gameReady = false;
    std::optional<Button> clickedButton;
    while (!gameReady && window.isOpen()) {



        
        clickEvent = joinGameGUI.processEventsAndReturnOnClick(window);
        window.clear();
        joinGameGUI.drawBackground(window);

        clickedButton = joinGameGUI.renderButtons(window, (clickEvent == 2));

        joinGameGUI.renderTextBoxes(window, (clickEvent == 2));

        if (clickedButton.has_value() && clickedButton.value().textString == "Join") {
            sf::TcpSocket socket;

            // Connect to the server (change this to your server's IP address)
            if (socket.connect(joinGameGUI.textBoxes[0].text, std::stoi(joinGameGUI.textBoxes[1].text)) != sf::Socket::Done) {
                std::cerr << "Failed to connect to the server!" << std::endl;
                return;
            }
            std::cout << "Connected to server!" << std::endl;
            // Receive the message from the server
            char buffer[128];
            std::size_t received;
            if (socket.receive(buffer, sizeof(buffer), received) != sf::Socket::Done) {
                std::cerr << "Failed to receive message!" << std::endl;
            }
            std::cout << "Received from server: " << buffer << std::endl;

            // Send a message to the server
            const std::string message = "Hello from the client!";
            if (socket.send(message.c_str(), message.size() + 1) != sf::Socket::Done) {
                std::cerr << "Failed to send message!" << std::endl;
            }
            
            // Launch multiplayer game GUI for the joiner (client plays black, for example)
            renderMultiplayerGameGUI(black, &socket);
            

            }
        window.draw(guideRect);
        window.draw(joinGuideString);
        window.display();
    }
}

// ----------renderMultiplayerGameGUI----------
// Note: This version now takes pointers to the ENetPeer and ENetHost
void renderMultiplayerGameGUI(PieceColor color,sf::TcpSocket* socket) {
    int clickEvent = -1;
    Chessboard board("rnbqkbnr/pppppppp/8/8/4N3/8/PPPPPPPP/RNBQKB1R w KQkq - 0 1");
    std::cout << "check" << std::endl;
    MultiplayerChessGUI multiplayerGameGUI(color, board);
    multiplayerGameGUI.font = font;
    Button printFENButton = Button(sf::Vector2f(windowSize.x * 0.88, windowSize.y * 0.5), font, "PRINT FEN", 50, sf::Color(255, 255, 255), sf::Color(200, 200, 200), 80);

    multiplayerGameGUI.buttons.emplace_back(printFENButton);

    multiplayerGameGUI.chessboard.calculateMoves();

    sf::Vector2i boardOffset(500, 50);
    std::optional<Button> clickedButton = std::nullopt;
    std::optional<Move> moveMade = std::nullopt;
    // Setup socket selector to monitor the socket for reading
    sf::SocketSelector selector;
    selector.add(*socket);  // Add socket to selector


    while (window.isOpen()) {
        window.clear();

        clickEvent = multiplayerGameGUI.updateClickEvent(clickEvent,
            multiplayerGameGUI.processEventsAndReturnOnClick(window));

        sf::Vector2i currentCoords = sf::Mouse::getPosition(window);

        // Assume processClick now returns an optional<Move> if a move was made
        moveMade = multiplayerGameGUI.processClick(clickEvent, window, boardOffset);

        multiplayerGameGUI.drawChessBoard(window, boardOffset);
        multiplayerGameGUI.drawSelectedPieceSquare(window, boardOffset);
        multiplayerGameGUI.drawPieces(window, boardOffset);
        multiplayerGameGUI.drawSelectedPiece(clickEvent, window, boardOffset);
        multiplayerGameGUI.drawSelectedPiecePossibilities(clickEvent, window, boardOffset);

        clickedButton = multiplayerGameGUI.renderButtons(window, (clickEvent == 1));
        if (clickedButton.has_value()) {
            if (clickedButton.value().textString == "PRINT FEN") {
                std::cout << multiplayerGameGUI.chessboard.getFEN() << std::endl;
            }
        }


        //Handle multiplayer
        std::vector<uint8_t> buffer(1024);  // Allocate a buffer for up to 1024 bytes
        std::size_t received;
        // Set a timeout for socket activity (e.g., 100 milliseconds)
        
        if (multiplayerGameGUI.clientColor != multiplayerGameGUI.chessboard.toMove) {
            sf::Time timeout = sf::milliseconds(50);
            if (selector.wait(timeout)) {
                if (selector.isReady(*socket)) {
                    // Socket is ready to read
                    sf::Socket::Status status = socket->receive(buffer.data(), buffer.size(), received);
                    if (status == sf::Socket::Done) {
                        Move moveToDo = deserializeMove(buffer.data(), 6 * sizeof(int));
                        multiplayerGameGUI.chessboard.executeMove(moveToDo);
                        if (color == white) {
                            multiplayerGameGUI.chessboard.calculateWhiteMoves();
                        }
                        else {
                            multiplayerGameGUI.chessboard.calculateBlackMoves();
                        }
                    }
                }
            }
        }

        if (moveMade.has_value()) {
            std::vector<uint8_t> message = serializeMove(moveMade.value());
            socket->send(message.data(),message.size());
        }


        window.display();

    }
}

void processDebugCommand(int input, Chessboard& board);
void debugGUI() {
    Chessboard board("2bqk2r/ppppp1pp/1r3n2/1bn2p2/2P1P3/1QN2N2/PP1P1PPP/R1B1KB1R w KQk- - 0 1");
    board.calculateWhiteMoves();
    int userInput = -1;
    while (userInput != 0) {
        std::cout << "Enter your command (for help, press 1): ";
        std::cin >> userInput;
        processDebugCommand(userInput,board);
    }
}

void processDebugCommand(int input, Chessboard& board) {
    switch (input) {
    case 0: {
        std::cout << "Exiting..." << std::endl;
        break;
    }
    case 1: {
        std::cout << "The following commands are allowed: " << std::endl;
        std::cout << "- Press 0 to exit the CLI" << std::endl;
        std::cout << "- Press 1 for help" << std::endl;
        std::cout << "- Press 2 to enter counting moves mode" << std::endl;
        std::cout << "- Press 3 to enter move executing mode" << std::endl;
        std::cout << "- Press 4 to get FEN" << std::endl;
        std::cout << "- Press 5 to set FEN" << std::endl;
        break;
    }
    case 2:{
        std::cout << "Entered counting moves mode.." << std::endl;
        std::cout << "Enter the depth of the search: ";
        int depth;
        std::cin >> depth;
        if (depth > 0) {
            std::array <Move, MAX_MOVES>  movesToCheck;
            int totalMoves;
            if (board.toMove == white) {
                board.calculateWhiteMoves();
                movesToCheck = board.whiteMoves;
                totalMoves = board.whiteMovesCount;
            }
            else {
                board.calculateBlackMoves();
                movesToCheck = board.blackMoves;
                totalMoves = board.blackMovesCount;
            }
            std::sort(movesToCheck.begin(), movesToCheck.begin() + totalMoves, [](Move& a, Move& b) {
                return a.toString() < b.toString();
                });

            long total = 0;
            for (int i = 0; i < totalMoves; i++) {
                Move currMove = movesToCheck[i];
                board.executeMove(currMove);
                long val = board.countMoves(depth - 1);
                total += val;
                std::cout << "Move: " << currMove.toString() <<" "<< val << std::endl;
                board.undoMove(currMove);
            }
            std::cout << "Total moves: " << total << std::endl;
        }
        break;
    }
    case 3: {
        std::cout << "Entered counting executing moves mode.." << std::endl;
        std::cout << "Enter the move to execute: ";
        std::string moveToExecute;
        std::cin >> moveToExecute;
        Move move = board.stringToMove(moveToExecute);
        std::cout << "Executing move : " << move.toString() << std::endl;
        board.executeMove(move);
        break;
    }
    case 4: {
        std::cout << board.getFEN() << std::endl;
        break;
    }
    case 5:{
        std::string val;
        std::cout << "Input your FEN: ";
        std::cin.ignore(); // Clear any leftover newline from previous input
        std::getline(std::cin, val);
        board.setFEN(val);
        std::cout << "FEN set." << std::endl;
        break;
    }
    }
    
    
}

int main()
{
    if (DEBUG) {
        debugGUI();
    }
    else {
        window.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "CHESS GAME", sf::Style::Default);
        windowSize = sf::Vector2f(window.getSize().x, window.getSize().y);

        window.setPosition(sf::Vector2i(0, 0));
        font.loadFromFile(".\\font\\arial.ttf");
        window.setMouseCursorVisible(true);
        renderStartGUI();
    }
    return 0;
}


std::vector<uint8_t> serializeMove(const Move& move) {
    std::vector<uint8_t> buffer;

    // Convert enum fields to int (assuming they are enums)
    int pieceTypeVal = static_cast<int>(move.pieceType);
    int pieceColorVal = static_cast<int>(move.pieceColor);
    int from0 = std::get<0>(move.from);
    int from1 = std::get<1>(move.from);
    int to0 = std::get<0>(move.to);
    int to1 = std::get<1>(move.to);

    // Helper lambda to append an int's bytes to the buffer
    auto appendInt = [&buffer](int value) {
        uint8_t bytes[sizeof(int)];
        std::memcpy(bytes, &value, sizeof(int));
        buffer.insert(buffer.end(), bytes, bytes + sizeof(int));
        };

    appendInt(pieceTypeVal);
    appendInt(pieceColorVal);
    appendInt(from0);
    appendInt(from1);
    appendInt(to0);
    appendInt(to1);

    return buffer;
}

Move deserializeMove(const uint8_t* data, size_t dataLength) {
    // Check if the packet is large enough: we expect 6 ints
    if (dataLength < 6 * sizeof(int)) {
        std::cerr << "Received packet is too small to contain a valid Move." << std::endl;
        return Move(); // return a default move or handle error
    }
    int pieceTypeVal, pieceColorVal, from0, from1, to0, to1;
    std::memcpy(&pieceTypeVal, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&pieceColorVal, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&from0, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&from1, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&to0, data, sizeof(int));
    data += sizeof(int);
    std::memcpy(&to1, data, sizeof(int));

    return Move(
        static_cast<PieceType>(pieceTypeVal),
        static_cast<PieceColor>(pieceColorVal),
        std::make_pair(from0, from1),
        std::make_pair(to0, to1)
    );
}

