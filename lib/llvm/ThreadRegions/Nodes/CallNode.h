#ifndef CALLNODE_H
#define CALLNODE_H

#include "Node.h"

class CallNode : public Node {
    std::set<Node *> directSuccessors_;

  public:
    CallNode(const llvm::Instruction *instruction = nullptr,
             const llvm::CallInst *callInst = nullptr);

    bool addDirectSuccessor(Node *callSuccessor);
    const std::set<Node *> &getDirectSuccessors() const;

    std::set<Node *> directSuccessors() const override;
};

#endif // CALLNODE_H
