#include "Move.h"

#pragma once

class MoveNode{
    public:
        Move value;
        std::vector<MoveNode> children;
}

class MoveTree{
    public:
    MoveNode root;
    
}
