//
// Created by Thierry Backes on 2019-06-27.
//

#ifndef PBRT_WHITTED_MATTE_H
#define PBRT_WHITTED_MATTE_H

#include <core/material.h>
#include <core/spectrum.h>
#include "reflection.h"
namespace pbrt{
    class MatteMaterial : public Material {
    public:
        MatteMaterial(const std::shared_ptr<Texture<Spectrum>> &Kd,
                        const std::shared_ptr<Texture<float>> &sigma)
        : Kd(Kd), sigma(sigma) {}

        void ComputeScatteringFunctions(SurfaceInteraction *si, MemoryArena &arena,
                                        TransportMode mode,
                                        bool allowMultipleLobes) const;

    private:
        std::shared_ptr<Texture<Spectrum>> Kd;
        std::shared_ptr<Texture<float>> sigma;
    };
}
#endif //PBRT_WHITTED_MATTE_H
