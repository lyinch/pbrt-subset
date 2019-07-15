//
// Created by Thierry Backes on 2019-07-15.
//

#ifndef PBRT_WHITTED_BVH_H
#define PBRT_WHITTED_BVH_H

#include "main.h"
#include "primitive.h"
#include <atomic>

namespace pbrt{
    struct BVHBuildNode;

    struct BVHPrimitiveInfo;

    class BVHAccel: public Aggregate{
    public:
        enum class SplitMethod {Middle, EqualCounts};
        BVHAccel(std::vector<std::shared_ptr<Primitive>> p,
                 int maxPrimsInNode = 1,
                 SplitMethod splitMethod = SplitMethod::Middle);
        BVHBuildNode *recursiveBuild(
                MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo,
                int start, int end, int *totalNodes,
                std::vector<std::shared_ptr<Primitive>> &orderedPrims);
        const int maxPrimsInNode;
        const SplitMethod splitMethod;
        std::vector<std::shared_ptr<Primitive>> primitives;
    };
}
#endif //PBRT_WHITTED_BVH_H
