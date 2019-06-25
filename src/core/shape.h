//
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_SHAPE_H
#define PBRT_WHITTED_SHAPE_H

#include "transform.h"
#include "geometry.h"
#include "main.h"

namespace pbrt{
    class Shape{
    public:
        Shape(const Transform *ObjectToWorld, const Transform *WorldToObject);
        virtual Bounds3f ObjectBound() const = 0;
        virtual Bounds3f WorldBound() const;
        virtual bool Intersect(const Ray &ray, float *tHit,
                               SurfaceInteraction *isect,
                               bool testAlphaTexture = true) const = 0;
        virtual bool IntersectP(const Ray &ray,
                                bool testAlphaTexture = true) const {
            return Intersect(ray, nullptr, nullptr, testAlphaTexture);
        }
        const Transform *ObjectToWorld, *WorldToObject;
    };
}
#endif //PBRT_WHITTED_SHAPE_H
