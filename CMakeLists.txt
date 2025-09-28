cmake_minimum_required(VERSION 3.10)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mpopcnt")

# All your source files
set(SOURCES
    Engine/TTEntry.cpp
    Engine/Board.cpp
    Engine/PieceType.cpp
    Engine/pieceColor.cpp
    Engine/MoveGenerator.cpp
    Engine/Move.cpp
    Engine/Search.cpp
    Engine/Evaluator.cpp
    main.cpp
)

add_executable(ChessEngine ${SOURCES})

