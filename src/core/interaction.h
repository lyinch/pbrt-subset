 //
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_INTERACTION_H
#define PBRT_WHITTED_INTERACTION_H

#include "main.h"
#include "geometry.h"
#include "transform.h"
#include "material.h"
#include "spectrum.h"

 namespace pbrt {
     struct Interaction {
         Interaction() = default;

         Interaction(const Point3f &p, const Normal3f &n, const Vector3f &wo) :
                 p(p), wo(wo), n(n) {}
         Interaction(const Point3f &p)
                 : p(p) {}
         Ray SpawnRay(const Vector3f &d) const {
             Point3f o = OffsetRayOrigin(p, n, d);
             return Ray(o, d, Infinity);
         }
         Ray SpawnRayTo(const Interaction &it) const {
             Point3f origin = OffsetRayOrigin(p, n, it.p - p);
             Point3f target = OffsetRayOrigin(it.p, it.n, origin - it.p);
             Vector3f d = target - origin;
             return Ray(origin, d, 1 - ShadowEpsilon);
         }
         Point3f p;
         Vector3f wo;
         Normal3f n;

     };

     class SurfaceInteraction : public Interaction {
     public:
         SurfaceInteraction() {}
         SurfaceInteraction(const Point3f &p,
                            const Point2f &uv, const Vector3f &wo,
                            const Vector3f &dpdu, const Vector3f &dpdv,
                            const Normal3f &dndu, const Normal3f &dndv,
                            const Shape *sh,
                            int faceIndex = 0);
         void ComputeScatteringFunctions(
                 const RayDifferential &ray, MemoryArena &arena,
                 bool allowMultipleLobes = false,
                 TransportMode mode = TransportMode::Radiance);
         void ComputeDifferentials(const RayDifferential &r) const;
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
         BSDF *bsdf = nullptr;
         const Primitive *primitive = nullptr;
         mutable Vector3f dpdx, dpdy;
         mutable float dudx = 0, dvdx = 0, dudy = 0, dvdy = 0;
     };
 }

#endif //PBRT_WHITTED_INTERACTION_H
