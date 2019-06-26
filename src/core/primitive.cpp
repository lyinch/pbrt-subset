//
// Created by Thierry Backes on 2019-06-26.
//

#include "primitive.h"
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
}