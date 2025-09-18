
#include <iostream>
#include <optional>
/*
#include "GUI.h"
#include "Button.h"
#include "GUI_Screens.h"
#include "ChessGUI.h"
#include "MultiplayerChessGUI.h"
#include "TextBox.h"
*/
#include "Engine/Board.h"
#include <bitset>
#include <chrono>
#include <thread>
#include <math.h>
#include <sstream>
#include <cstdlib>
#include <fstream>

int main()
{
    Board board = Board();
    board.setStartingPosition();
    board.print();
    
}
