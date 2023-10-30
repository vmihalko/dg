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
    // change all nodes such that they explicitely have these predecessors???
    return true;
}

Node *CallNode::getCallSuccessor() const { return callSuccessor_; }

void CallNode::printOutcomingEdges(std::ostream &ostream) const {
    Node::printOutcomingEdges(ostream);
    if (callSuccessor_ != nullptr) {
        ostream << this->dotName() << " -> " << callSuccessor_->dotName()
                << " [style=dotted]";
    }
}
