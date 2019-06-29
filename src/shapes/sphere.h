//
// Created by Thierry Backes on 2019-06-26.
//

#ifndef PBRT_WHITTED_SPHERE_H
#define PBRT_WHITTED_SPHERE_H

#include <core/shape.h>

namespace pbrt{
    class Sphere : public Shape {
    public:
        Sphere(const Transform *ObjectToWorld, const Transform *WorldToObject,
               float radius, float zMin, float zMax,
               float phiMax):Shape(ObjectToWorld, WorldToObject),
                             radius(radius),
                             zMin(Clamp(std::min(zMin, zMax), -radius, radius)),
                             zMax(Clamp(std::max(zMin, zMax), -radius, radius)),
                             thetaMin(std::acos(Clamp(std::min(zMin, zMax) / radius, -1, 1))),
                             thetaMax(std::acos(Clamp(std::max(zMin, zMax) / radius, -1, 1))),
                             phiMax(Radians(Clamp(phiMax, 0, 360))) {}
        Bounds3f ObjectBound() const;

        bool Intersect(const Ray &r, float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture) const;

    private:
        const float radius;
        const float zMin, zMax;
        const float thetaMin, thetaMax, phiMax;

    };
    std::shared_ptr<Shape> CreateSphereShape(const Transform *o2w,
                                             const Transform *w2o);
    }
#endif //PBRT_WHITTED_SPHERE_H
