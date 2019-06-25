 //
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_INTERACTION_H
#define PBRT_WHITTED_INTERACTION_H

#include "geometry.h"
#include "shape.h"
#include "memory.h"

 namespace pbrt {
     struct Interaction {
         Interaction() = default;

         Interaction(const Point3f &p, const Normal3f &n, const Vector3f &pError, const Vector3f &wo) :
                 p(p), pError(pError), wo(wo), n(n) {}

         Ray SpawnRay(const Vector3f &d) const {
             Point3f o = OffsetRayOrigin(p, pError, n, d);
             return Ray(o, d, Infinity);
         }
         Point3f p;
         Vector3f pError;
         Vector3f wo;
         Normal3f n;

     };

     class SurfaceInteraction : public Interaction {
     public:

         SurfaceInteraction() {}
         SurfaceInteraction(const Point3f &p, const Vector3f &pError,
                            const Point2f &uv, const Vector3f &wo,
                            const Vector3f &dpdu, const Vector3f &dpdv,
                            const Normal3f &dndu, const Normal3f &dndv,
                            const Shape *sh,
                            int faceIndex = 0);
         void ComputeScatteringFunctions(
                 const RayDifferential &ray, MemoryArena &arena,
                 bool allowMultipleLobes = false,
                 TransportMode mode = TransportMode::Radiance);
         Spectrum Le(const Vector3f &w) const;

         Point2f uv;
         Vector3f dpdu, dpdv;
         Normal3f dndu, dndv;
         const Shape *shape = nullptr;
         struct {
             Normal3f n;
             Vector3f dpdu, dpdv;
             Normal3f dndu, dndv;
         } shading;
         const Primitive *primitive = nullptr;

     };
 }

#endif //PBRT_WHITTED_INTERACTION_H
