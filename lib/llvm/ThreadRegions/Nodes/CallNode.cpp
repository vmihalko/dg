#include "CallNode.h"

#include <iostream>

CallNode::CallNode(const llvm::Instruction *instruction,
                   const llvm::CallInst *callInst)
        : Node(NodeType::CALL, instruction, callInst) {}

bool CallNode::setCallSuccessor(Node *callSuccessor) {
    if (callSuccessor_ != nullptr) {
        return false;
    }

    callSuccessor_ = callSuccessor;
    callSuccessor_->addCallPredecessor(this);
    return true;
}

Node *CallNode::getCallSuccessor() const { return callSuccessor_; }

std::set<Node *> CallNode::directSuccessors() const {
    if (callSuccessor_ == nullptr) {
        return successors();
    }
    return { callSuccessor_ };
}

