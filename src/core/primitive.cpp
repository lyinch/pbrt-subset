//
// Created by Thierry Backes on 2019-06-26.
//

#include "primitive.h"
#include "light.h"
#include "interaction.h"
namespace pbrt{

    Primitive::~Primitive() {}

    Bounds3f GeometricPrimitive::WorldBound() const {
        return shape->WorldBound();
    }

    GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<Shape> &shape,
                                           const std::shared_ptr<Material> &material):
                                           shape(shape),
                                            material(material) {
    }

    bool GeometricPrimitive::Intersect(const Ray &r, SurfaceInteraction *isect) const {
        float tHit;
        if (!shape->Intersect(r, &tHit, isect)) return false;
        r.tMax = tHit;
        isect->primitive = this;
        return true;
    }

    bool GeometricPrimitive::IntersectP(const Ray &r) const {
        return shape->IntersectP(r);
    }
    void GeometricPrimitive::ComputeScatteringFunctions(
            SurfaceInteraction *isect, MemoryArena &arena, TransportMode mode,
            bool allowMultipleLobes) const {
        if (material)
            material->ComputeScatteringFunctions(isect, arena, mode,
                                                 allowMultipleLobes);
    }
}