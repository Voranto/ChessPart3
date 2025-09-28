#include "Move.h"
#include <vector>
#pragma once

class MoveNode{
    public:
        Move value;
        std::vector<MoveNode> children;
        MoveNode(Move val);
        MoveNode();


    bool operator==(const MoveNode& other) const {
		return this->value == other.value;
	}
	bool operator!=(const MoveNode& other) const {
        return !(*this == other);
    }
};

class MoveTree{
    public:
    MoveNode  root;



};
