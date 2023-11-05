#ifndef CONTROLFLOWGRAPH_H
#define CONTROLFLOWGRAPH_H

#include "dg/llvm/ThreadRegions/MayHappenInParallel.h"
#include <memory>
#include <set>

#include <llvm/IR/Instructions.h>

namespace dg {
class DGLLVMPointerAnalysis;
}

namespace llvm {
class Function;
}

class ThreadRegion;
class GraphBuilder;
class ThreadRegionsBuilder;
class CriticalSectionsBuilder;

class ControlFlowGraph {
  private:
    std::unique_ptr<GraphBuilder> graphBuilder;
    std::unique_ptr<ThreadRegionsBuilder> threadRegionsBuilder;
    std::unique_ptr<CriticalSectionsBuilder> criticalSectionsBuilder;

  public:
    ControlFlowGraph(dg::DGLLVMPointerAnalysis *pointsToAnalysis);

    ~ControlFlowGraph();

    std::set<const llvm::CallInst *> getJoins() const;

    std::set<const llvm::CallInst *>
    getCorrespondingForks(const llvm::CallInst *callInst) const;

    std::set<const llvm::CallInst *> getLocks() const;

    std::set<const llvm::CallInst *>
    getCorrespongingUnlocks(const llvm::CallInst *callInst) const;

    std::set<const llvm::Instruction *>
    getCorrespondingCriticalSection(const llvm::CallInst *callInst) const;

    void buildFunction(const llvm::Function *function);

    void printWithRegions(std::ostream &ostream,
                          const MayHappenInParallel *mhp = nullptr) const;

    void printWithoutRegions(std::ostream &ostream) const;

    ThreadRegion *mainEntryRegion() const;
    std::set<ThreadRegion *> allRegions() const;
};

#endif // CONTROLFLOWGRAPH_H
