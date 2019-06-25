//
// Created by Thierry Backes on 2019-06-19.
//

#ifndef PBRT_WHITTED_INTEGRATOR_H
#define PBRT_WHITTED_INTEGRATOR_H

#include "scene.h"
#include "sampler.h"
#include "camera.h"

namespace pbrt{
    class Integrator{
    public:
        virtual ~Integrator();
        virtual void Render(const Scene &scene) = 0;
    };

    class SamplerIntegrator: public Integrator{
    public:
        SamplerIntegrator(std::shared_ptr<const Camera> camera,
                          std::shared_ptr<Sampler> sampler,
                          const Bounds2i &pixelBounds)
                : camera(camera), sampler(sampler), pixelBounds(pixelBounds) {}

        void Render(const Scene &scene) override;

    protected:
        std::shared_ptr<const Camera> camera;

    private:
        std::shared_ptr<Sampler> sampler;
        const Bounds2i pixelBounds;

    };
}
#endif //PBRT_WHITTED_INTEGRATOR_H
