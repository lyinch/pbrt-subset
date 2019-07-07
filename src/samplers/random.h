//
// Created by Thierry Backes on 2019-06-18.
//

#ifndef PBRT_WHITTED_RANDOM_H
#define PBRT_WHITTED_RANDOM_H

#include <core/sampler.h>
#include "rng.h"
#include <memory>

namespace pbrt{
    class RandomSampler : public Sampler{
    public:
        RandomSampler(int ns, int seed = 0);
        void StartPixel(const Point2i &);
        float Get1D();
        Point2f Get2D();
        std::unique_ptr<Sampler> Clone(int seed);

    private:
        RNG rng;

    };
    Sampler *CreateRandomSampler();

}

#endif //PBRT_WHITTED_RANDOM_H
