cmake_minimum_required(VERSION 3.10)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# All your source files
set(SOURCES
    Engine/Board.cpp
    Engine/PieceType.cpp
    Engine/pieceColor.cpp
    Engine/MoveGenerator.cpp
    main.cpp
)

add_executable(ChessEngine ${SOURCES})

