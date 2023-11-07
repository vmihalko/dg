#ifndef MAYHAPPENINPARALLEL_H
#define MAYHAPPENINPARALLEL_H

#include <map>
#include <ostream>
#include <set>
#include <utility>

#include "ThreadRegion.h"

class MayHappenInParallel {
    using MHPPair = std::pair<const ThreadRegion *, const ThreadRegion *>;
    using MHPRelationGraph = std::set<MHPPair>;

    const ThreadRegion *rootRegion_;
    std::map<const ThreadRegion *, std::set<const ThreadRegion *>> mhpInfo_;
    size_t relationCount_ = 0;

    static const std::set<const ThreadRegion *> emptyRegion_;

  public:
    MayHappenInParallel(const ThreadRegion *rootRegion);

    void run();

    const std::set<const ThreadRegion *> &
    parallelRegions(const ThreadRegion *threadRegion);

    void printEdges(std::ostream &ostream) const;
    size_t countRelations() const;

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
};

#endif // MAYHAPPENINPARALLEL_H
