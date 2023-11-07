#include "dg/llvm/ThreadRegions/MayHappenInParallel.h"
#include "llvm/ThreadRegions/Nodes/Node.h"

#include <llvm/IR/CFG.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

const std::set<const ThreadRegion *> MayHappenInParallel::emptyRegion_ = {};

MayHappenInParallel::MayHappenInParallel(const ThreadRegion *rootRegion)
        : rootRegion_(rootRegion) {}

void MayHappenInParallel::run() {
    MHPRelationGraph res = runAnalysis();

    for (auto pair : res) {
        // the set must exist; I will have to fill it in another method
        mhpInfo_[pair.first].insert(pair.second);
        mhpInfo_[pair.second].insert(pair.first);
    }

    relationCount_ = res.size();
}

const std::set<const ThreadRegion *> &
MayHappenInParallel::parallelRegions(const ThreadRegion *region) {
    if (mhpInfo_.find(region) == mhpInfo_.end()) {
        // this is ugly, but it works...
        return emptyRegion_;
    }

    return mhpInfo_[region];
}

void MayHappenInParallel::printEdges(std::ostream &ostream) const {
    for (const auto &pair : mhpInfo_) {
        const ThreadRegion *region = pair.first;
        const std::set<const ThreadRegion *> &successors = pair.second;

        for (const auto *successor : successors) {
            // we do not want to add the edges twice
            if (successor < region) {
                continue;
            }

            ostream << region->firstNode()->dotName() << " -> "
                << successor->firstNode()->dotName()
                << " [ltail = " << region->dotName()
                << " lhead = " << successor->dotName()
                << ", color = chartreuse, fontcolor = chartreuse, style = bold"
                << ", label=\"MHP\"]";
        }
    }
}

size_t MayHappenInParallel::countRelations() const {
    return relationCount_;
}

// these are only the relations which are the immediate consequence of FORKs
MayHappenInParallel::MHPRelationGraph
MayHappenInParallel::findInitialMHP() const {
    std::vector<const ThreadRegion *> worklist = {rootRegion_};
    std::set<const ThreadRegion *> visited = {};
    MHPRelationGraph res = {};

    while (!worklist.empty()) {
        const auto *current = worklist.back();
        worklist.pop_back();

        for (const auto *succ : current->directSuccessors()) {
            if (visited.find(succ) == visited.end()) {
                worklist.push_back(succ);
                visited.insert(succ);
            }
        }

        for (auto pair : current->forkedSuccessors()) {
            for (const auto *succ : current->directSuccessors()) {
                res.insert(createMHPPair(succ, pair.second));
            }
        }

        const auto *callSucc = current->interestingCallSuccessor();

        if (callSucc != nullptr && visited.find(callSucc) == visited.end()) {
            if (!current->forkedSuccessors().empty()) {
                worklist.push_back(current->interestingCallSuccessor());
                visited.insert(current->interestingCallSuccessor());
            }
        }
    }

    return res;
}

MayHappenInParallel::MHPRelationGraph MayHappenInParallel::refineMHP(
        MayHappenInParallel::MHPRelationGraph mhpGraph) const {
    // may be refined in the future by considering mutexes
    return mhpGraph;
}

MayHappenInParallel::MHPRelationGraph MayHappenInParallel::runAnalysis() const {
    auto mhp = findInitialMHP();
    std::vector<MHPPair> worklist;

    worklist.insert(worklist.end(), mhp.begin(), mhp.end());

    while (!worklist.empty()) {
        auto current = worklist.back();
        worklist.pop_back();

        const auto *first = current.first;
        const auto *second = current.second;

        for (int i = 0; i < 2; i++) {
            for (const auto *succ : first->directSuccessors()) {
                updateMHPGraph(mhp, worklist, createMHPPair(succ, second));

                for (auto pair : first->forkedSuccessors()) {
                    updateMHPGraph(mhp, worklist,
                                   createMHPPair(succ, pair.second));
                }
            }

            for (const auto *callSucc : first->calledSuccessors()) {
                updateMHPGraph(mhp, worklist, createMHPPair(callSucc, second));
            }

            const auto *interestingSucc = first->interestingCallSuccessor();
            if (interestingSucc != nullptr) {
                updateMHPGraph(mhp, worklist,
                               createMHPPair(interestingSucc, second));
            }

            for (auto pair : first->forkedSuccessors()) {
                updateMHPGraph(mhp, worklist, createMHPPair(pair.second, second));
            }

            std::swap(first, second);
        }
    }

    return refineMHP(mhp);
}

void MayHappenInParallel::updateMHPGraph(
        MayHappenInParallel::MHPRelationGraph &mhp,
        std::vector<MHPPair> &worklist, MHPPair newPair) {
    if (mhp.find(newPair) == mhp.end()) {
        mhp.insert(newPair);
        worklist.push_back(newPair);
    }
}
