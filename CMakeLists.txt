cmake_minimum_required(VERSION 3.10)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mpopcnt")
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
add_definitions(-DSFML_STATIC)
# All your source files
set(SOURCES
    GUI/Piece.cpp
    GUI/GUI.cpp
    GUI/ChessGUI.cpp
    GUI/TextBox.cpp
    GUI/Button.cpp
    Engine/TTEntry.cpp
    Engine/Board.cpp
    Engine/PieceType.cpp
    Engine/pieceColor.cpp
    Engine/MoveGenerator.cpp
    Engine/Move.cpp
    Engine/MoveTree.cpp
    Engine/Search.cpp
    Engine/Evaluator.cpp
    main.cpp
)

# Define the executable first
add_executable(ChessEngine ${SOURCES})

# Include directories
target_include_directories(ChessEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# --- SFML 3 setup ---
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

target_link_libraries(ChessEngine PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
