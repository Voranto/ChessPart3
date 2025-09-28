#include "MoveTree.h"

MoveNode::MoveNode(Move val){
    this->value = val;
    this->children = {};
}

MoveNode::MoveNode(){
    this->value = Move();
    this->children = {};
}