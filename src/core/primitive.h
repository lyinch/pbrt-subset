//
// Created by Thierry Backes on 2019-06-26.
//

#ifndef PBRT_WHITTED_PRIMITIVE_H
#define PBRT_WHITTED_PRIMITIVE_H

#include "main.h"
#include "shape.h"
#include "material.h"
#include "transform.h"

namespace pbrt{
    class Primitive {
    public:
        virtual ~Primitive();
        virtual Bounds3f WorldBound() const = 0;
        virtual bool Intersect(const Ray &r, SurfaceInteraction *) const = 0;
        virtual bool IntersectP(const Ray &r) const = 0;
        virtual void ComputeScatteringFunctions(SurfaceInteraction *isect,
                                                MemoryArena &arena,
                                                TransportMode mode,
                                                bool allowMultipleLobes) const = 0;
    };

    class GeometricPrimitive: public Primitive {
    public:
        virtual Bounds3f WorldBound() const;
        virtual bool Intersect(const Ray &r, SurfaceInteraction *isect) const;
        virtual bool IntersectP(const Ray &r) const;
        GeometricPrimitive(const std::shared_ptr<Shape> &shape,
                           const std::shared_ptr<Material> &material);
        virtual void ComputeScatteringFunctions(SurfaceInteraction *isect,
                                                MemoryArena &arena,
                                                TransportMode mode,
                                                bool allowMultipleLobes) const;
    private:
        // GeometricPrimitive Private Data
        std::shared_ptr<Shape> shape;
        std::shared_ptr<Material> material;
    };

    class Aggregate : public Primitive {
        
    };
}
#endif //PBRT_WHITTED_PRIMITIVE_H
