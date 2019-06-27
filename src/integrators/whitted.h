//
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_WHITTED_H
#define PBRT_WHITTED_WHITTED_H

#include "main.h"
#include "integrator.h"
#include "scene.h"

namespace pbrt {
    class WhittedIntegrator : public SamplerIntegrator {
    public:
        WhittedIntegrator(int maxDepth, std::shared_ptr<const Camera> camera, std::shared_ptr<Sampler> sampler, const Bounds2i &pixelBounds):
        SamplerIntegrator(camera,sampler,pixelBounds), maxDepth(maxDepth){}
        Spectrum Li(const RayDifferential &ray, const Scene &scene,
                    Sampler &sampler, MemoryArena &arena, int depth) const;
    private:
        const int maxDepth;
    };
    WhittedIntegrator *CreateWhittedIntegrator(
            std::shared_ptr<Sampler> sampler,
            std::shared_ptr<const Camera> camera);
}
#endif //PBRT_WHITTED_WHITTED_H
