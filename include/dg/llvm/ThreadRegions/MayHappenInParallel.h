#ifndef MAYHAPPENINPARALLEL_H
#define MAYHAPPENINPARALLEL_H

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "ThreadRegion.h"

class MayHappenInParallel {
    // the representation may change in the future (fork information...)
    // TODO: rewrite it to trivially consider forks
    using MHPPair = std::pair<const ThreadRegion *, const ThreadRegion *>;
    using MHPRelationGraph = std::set<MHPPair>;

    const ThreadRegion *rootRegion_;
    std::map<const ThreadRegion *, std::set<const ThreadRegion *>> mhpInfo_;

    static const std::set<const ThreadRegion *> emptyRegion_;

  public:
    MayHappenInParallel(const ThreadRegion *rootRegion);

    void run();

    const std::set<const ThreadRegion *> &
    parallelRegions(const ThreadRegion *threadRegion);

  private:
    // we do this to save space, as the MHP relation is symetric
    static MHPPair createMHPPair(const ThreadRegion *first,
                                 const ThreadRegion *second) {
        if (first < second) {
            return {first, second};
        }

        return {second, first};
    }

    // warning: this is only used internally, the result may be an
    // underestimation of the real MHP relation
    MHPRelationGraph findInitialMHP() const;

    MHPRelationGraph runAnalysis() const;

    // if the input mhpGraph is a correct overapproximation of the real MHP
    // relation, then the result of this method is also a correct
    // overapproximation of the MHP relation
    MHPRelationGraph refineMHP(MHPRelationGraph mhpGraph) const;

    static void updateMHPGraph(MHPRelationGraph &mhp,
                               std::vector<MHPPair> &worklist,
                               MHPPair newPair);
};

#endif // MAYHAPPENINPARALLEL_H
