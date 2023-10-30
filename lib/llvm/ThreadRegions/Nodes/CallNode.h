#ifndef CALLNODE_H
#define CALLNODE_H

#include "Node.h"

class CallNode : public Node {
    Node *callSuccessor_;

  public:
    CallNode(const llvm::Instruction *instruction = nullptr,
             const llvm::CallInst *callInst = nullptr);

    bool setCallSuccessor(Node *callSuccessor);
    Node *getCallSuccessor() const;

    void printOutcomingEdges(std::ostream &ostream) const override;
};

#endif // CALLNODE_H
