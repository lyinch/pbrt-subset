//
// Created by Thierry Backes on 2019-06-19.
//

#include "shape.h"
namespace pbrt {
    Shape::Shape(const Transform *ObjectToWorld, const Transform *WorldToObject)
            : ObjectToWorld(ObjectToWorld), WorldToObject(WorldToObject) {}

    Bounds3f Shape::WorldBound() const { return (*ObjectToWorld)(ObjectBound()); }

}