//
// Created by Thierry Backes on 2019-06-19.
//

#include "scene.h"
namespace pbrt {
    bool Scene::Intersect(const pbrt::Ray &ray, pbrt::SurfaceInteraction *isect) const {
        return aggregate->Intersect(ray, isect);
    }

    bool Scene::IntersectP(const Ray &ray) const {
        return aggregate->IntersectP(ray);
    }
}
