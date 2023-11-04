#ifndef THREADREGIONSBUILDER_H
#define THREADREGIONSBUILDER_H

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "ConcurrencyProcedureAnalysis.h"
#include "dg/llvm/ThreadRegions/ThreadRegion.h"

// possible improvements:
// * add some subtypes of ThreadRegion - for example, when an uninteresting
//   procedure forms a single region, the region does not have to remember
//   all its nodes directly, as that can be easily calculated on demand

// thread region semantics:
// * all nodes in a thread region have the same MHP behaviour
// * the thread regions are sequential blocks - if there is
//   any branching, a new region is constructed
//     * there is one exception to this rule - if a procedure
//       is found to not have interesting MHP behaviour, the
//       full procedure can become a single thread region
class ThreadRegionsBuilder {
  private:
    std::unique_ptr<ConcurrencyProcedureAnalysis> concurrencyProcedureAnalysis_;

    std::vector<std::unique_ptr<ThreadRegion>> threadRegions_;
    std::unordered_map<Node *, ThreadRegion *> nodeToRegionMap_;

    std::vector<ThreadRegion *> worklist_;


  public:
    ThreadRegionsBuilder();

    void build(EntryNode *mainEntry, std::set<EntryNode *> &procedureEntries);

    void printNodes(std::ostream &ostream) const;
    void printEdges(std::ostream &ostream) const;

    void reserve(std::size_t size);
    void clear();

    ThreadRegion *entryRegion() const;
    std::set<ThreadRegion *> allRegions() const;

  private:

    // FIXME: add consts where they belong

    void insertNodeIntoRegion(ThreadRegion *region, Node *node);
    ThreadRegion *findOrCreateRegion(Node *node);
    bool isInteresting(Node *node) const;
    ThreadRegion *buildUninterestingProcedure(EntryNode *entryNode);
    bool regionIsComplete(Node *lastNode, std::set<Node *> &successors) const;
};

#endif // THREADREGIONSBUILDER_H
