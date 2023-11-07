#ifndef THREADREGION_H
#define THREADREGION_H

#include <iosfwd>
#include <set>
#include <vector>

class Node;
class ForkNode;
class ControlFlowGraph;

namespace llvm {
class Instruction;
}

class ThreadRegion {
    int id_;
    Node *foundingNode_;
    std::vector<Node *> nodes_;
    std::set<ThreadRegion *> predecessors_;
    std::set<ThreadRegion *> successors_;

    // would vectors be better here?
    std::set<const ThreadRegion *> directSuccessors_;
    std::set<const ThreadRegion *> calledSuccessors_;
    std::set<std::pair<const ForkNode *, const ThreadRegion *>>
            forkedSuccessors_;
    const ThreadRegion *interestingCallSuccessor_ = nullptr;

    static int lastId;

  public:
    ThreadRegion();
    ThreadRegion(Node *node);

    int id() const;

    bool addPredecessor(ThreadRegion *predecessor);
    bool addSuccessor(ThreadRegion *threadRegion);

    bool removePredecessor(ThreadRegion *predecessor);
    bool removeSuccessor(ThreadRegion *successor);

    const std::set<ThreadRegion *> &predecessors() const;
    std::set<ThreadRegion *> predecessors();

    const std::set<ThreadRegion *> &successors() const;
    std::set<ThreadRegion *> successors();

    // there are four types of successors based on which MHP
    // information will be propagated to them

    void addDirectSuccessor(const ThreadRegion *region);
    void addCallSuccessor(const ThreadRegion *region);
    void addForkedSuccessor(const ForkNode *forkNode,
                            const ThreadRegion *region);
    void setInterestingCallSuccessor(const ThreadRegion *region);

    // the direct successors are in the same procedure as
    // the very region; the MHP data propagates to them,
    // and they may get new MHP relations, depending on
    // other successors
    // the propagated MHP relation may be thrown away if
    // the successor is a join region, which always joins
    // the fork
    const std::set<const ThreadRegion *> &directSuccessors() const;

    // the called successors are the uninteresting calls from
    // inside of the region; only the MHP information from this
    // notes propagates to them directly
    const std::set<const ThreadRegion *> &calledSuccessors() const;

    // the called fork is a special kind of successor, as it
    // is not a thread region, but a node instead; it signifies
    // a fork which may be invoked at the end of this region
    // without certainly being joined
    const std::set<std::pair<const ForkNode *, const ThreadRegion *>> &
    forkedSuccessors() const;

    // FIXME: there can be more regions if I consider all CALL successors
    // of a CALL_PTR node to be part of the same region; in that case,
    // the number of interesting call successors can be greater than one

    // this successor is always at most one, and it is the entry
    // point of the interesting procedure called at the end of
    // the region; it may be null
    const ThreadRegion *interestingCallSuccessor() const;

    bool insertNode(Node *node);

    // I may yeet this in the future
    Node *foundingNode() const;

    // every region has exactly one first node and one
    // last node
    Node *firstNode() const;
    Node *lastNode() const;

    void printNodes(std::ostream &ostream);
    void printEdges(std::ostream &ostream);

    std::string dotName() const;
    /**
     * @brief returns all llvm instructions contained in the thread region
     * @return
     */
    std::set<const llvm::Instruction *> llvmInstructions() const;
};

#endif // THREADREGION_H
