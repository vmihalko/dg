#ifndef THREADREGIONGRAPH_H
#define THREADREGIONGRAPH_H

#include "dg/llvm/ThreadRegions/ThreadRegion.h"

class ThreadRegionGraph {
    ThreadRegion *entryRegion_;
    std::set<ThreadRegion *> regions_;

  public:
    ThreadRegionGraph();
    ~ThreadRegionGraph();

    ThreadRegion *entryRegion();
};

#endif // THREADREGIONGRAPH_H

