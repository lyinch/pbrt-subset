//
// Created by Thierry Backes on 2019-07-15.
//

#include "accelerators/bvh.h"
#include "interaction.h"
#include "parallel.h"
#include <algorithm>

namespace pbrt{

    struct BVHPrimitiveInfo {
        BVHPrimitiveInfo() {}
        BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
                : primitiveNumber(primitiveNumber),
                  bounds(bounds),
                  centroid(.5f * bounds.pMin + .5f * bounds.pMax) {}
        size_t primitiveNumber;
        Bounds3f bounds;
        Point3f centroid;
    };

    struct BVHBuildNode {
        // BVHBuildNode Public Methods
        void InitLeaf(int first, int n, const Bounds3f &b) {
            firstPrimOffset = first;
            nPrimitives = n;
            bounds = b;
            children[0] = children[1] = nullptr;
        }
        void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
            children[0] = c0;
            children[1] = c1;
            bounds = Union(c0->bounds, c1->bounds);
            splitAxis = axis;
            nPrimitives = 0;
        }
        Bounds3f bounds;
        BVHBuildNode *children[2];
        int splitAxis, firstPrimOffset, nPrimitives;
    };

    BVHAccel::BVHAccel(std::vector<std::shared_ptr<Primitive>> p, int maxPrimsInNode,
                      SplitMethod splitMethod):
                      maxPrimsInNode(std::min(255, maxPrimsInNode)),
                      splitMethod(splitMethod),
                      primitives(std::move(p)) {
        if (primitives.empty()) return;


        // Initialize _primitiveInfo_ array for primitives
        std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
        for (size_t i = 0; i < primitives.size(); ++i)
            primitiveInfo[i] = {i, primitives[i]->WorldBound()};

        // Build BVH tree for primitives using _primitiveInfo_
        MemoryArena arena(1024 * 1024);
        int totalNodes = 0;
        std::vector<std::shared_ptr<Primitive>> orderedPrims;
        orderedPrims.reserve(primitives.size());
        BVHBuildNode *root;

        root = recursiveBuild(arena, primitiveInfo, 0, primitives.size(),
                              &totalNodes, orderedPrims);
        primitives.swap(orderedPrims);
        primitiveInfo.resize(0);

    }

    BVHBuildNode *
    BVHAccel::recursiveBuild(MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo, int start, int end,
                             int *totalNodes, std::vector<std::shared_ptr<Primitive>> &orderedPrims) {
        BVHBuildNode *node = arena.Alloc<BVHBuildNode>();
        (*totalNodes)++;

        int nPrimitives = end - start;
        Bounds3f bounds;
        for (int i = start; i < end; ++i)
            bounds = Union(bounds, primitiveInfo[i].bounds);

        if (nPrimitives == 1) {
            int firstPrimOffset = orderedPrims.size();
            for (int i = start; i < end; ++i) {
                int primNum = primitiveInfo[i].primitiveNumber;
                orderedPrims.push_back(primitives[primNum]);
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
            return node;
        } else {
            // Compute bound of primitive centroids, choose split dimension _dim_
            Bounds3f centroidBounds;
            for (int i = start; i < end; ++i)
                centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
            int dim = centroidBounds.MaximumExtent();

            // Partition primitives into two sets and build children
            int mid = (start + end) / 2;
            if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim]) {
                // Create leaf _BVHBuildNode_
                int firstPrimOffset = orderedPrims.size();
                for (int i = start; i < end; ++i) {
                    int primNum = primitiveInfo[i].primitiveNumber;
                    orderedPrims.push_back(primitives[primNum]);
                }
                node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
                return node;
            } else {
                switch (splitMethod) {
                    case SplitMethod::Middle: {
                        // Partition primitives through node's midpoint
                        float pmid =
                                (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]) / 2;
                        BVHPrimitiveInfo *midPtr = std::partition(
                                &primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                                [dim, pmid](const BVHPrimitiveInfo &pi) {
                                    return pi.centroid[dim] < pmid;
                                });
                        mid = midPtr - &primitiveInfo[0];
                        // For lots of prims with large overlapping bounding boxes, this
                        // may fail to partition; in that case don't break and fall
                        // through
                        // to EqualCounts.
                        if (mid != start && mid != end) break;
                    }
                    case SplitMethod::EqualCounts: {
                        // Partition primitives into equally-sized subsets
                        mid = (start + end) / 2;
                        std::nth_element(&primitiveInfo[start], &primitiveInfo[mid],
                                         &primitiveInfo[end - 1] + 1,
                                         [dim](const BVHPrimitiveInfo &a,
                                               const BVHPrimitiveInfo &b) {
                                             return a.centroid[dim] < b.centroid[dim];
                                         });
                        break;
                    }
                } //end switch

                node->InitInterior(dim,
                                   recursiveBuild(arena, primitiveInfo, start, mid,
                                                  totalNodes, orderedPrims),
                                   recursiveBuild(arena, primitiveInfo, mid, end,
                                                  totalNodes, orderedPrims));
            }

        } //end if/else nPrimitives == 1
        return node;
    }
}