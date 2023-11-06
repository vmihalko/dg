#ifndef EXITNODE_H
#define EXITNODE_H

#include "Node.h"

#include <set>

class JoinNode;

class ExitNode : public Node {
  private:
    std::set<JoinNode *> joinSuccessors_;

  public:
    ExitNode();

    bool addJoinSuccessor(JoinNode *joinNode);

    bool removeJoinSuccessor(JoinNode *joinNode);

    const std::set<JoinNode *> &joinSuccessors() const;
    std::set<JoinNode *> joinSuccessors();

    std::size_t successorsNumber() const override;

    std::set<Node *> directSuccessors() const override;

    void printOutcomingEdges(std::ostream &ostream,
                             bool printOnlyDirect = false) const override;

    friend class JoinNode;
};

#endif // EXITNODE_H
