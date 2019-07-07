//
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_SCENE_H
#define PBRT_WHITTED_SCENE_H

#include "main.h"
#include "geometry.h"
#include "primitive.h"
#include "light.h"

namespace pbrt{
    class Scene{

    public:
        Scene(std::shared_ptr<Primitive> aggregate,
              const std::vector<std::shared_ptr<Light>> &lights) : lights(lights), aggregate(aggregate) {
            worldBound = aggregate->WorldBound();
        }

        const Bounds3f &WorldBound() const { return worldBound; }
        bool Intersect(const Ray &ray, SurfaceInteraction *isect) const;
        bool IntersectP(const Ray &ray) const;

        std::vector<std::shared_ptr<Light>> lights;
        std::vector<std::shared_ptr<Light>> infiniteLights;
    private:
        std::shared_ptr<Primitive> aggregate;
        Bounds3f worldBound; //Bounding box of scene

    };
}
#endif //PBRT_WHITTED_SCENE_H
