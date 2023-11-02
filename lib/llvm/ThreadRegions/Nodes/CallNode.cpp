#include "CallNode.h"

#include <iostream>

CallNode::CallNode(const llvm::Instruction *instruction,
                   const llvm::CallInst *callInst)
        : Node(NodeType::CALL, instruction, callInst) {}

bool CallNode::addDirectSuccessor(Node *callSuccessor) {
    directSuccessors_.insert(callSuccessor);
    callSuccessor->addCallPredecessor(this);
    return true;
}

const std::set<Node *> &CallNode::getDirectSuccessors() const {
    return directSuccessors_;
}

std::set<Node *> CallNode::directSuccessors() const {
    return directSuccessors_;
}

